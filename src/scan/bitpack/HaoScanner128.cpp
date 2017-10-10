/*
 * HaoScanner128.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include <immintrin.h>
#include <assert.h>
#include "../../util/math_util.h"
#include "HaoScanner128.h"

#define INT_LEN 32
#define SIMD_LEN 128
#define BYTE_LEN 8

const __m128i one = _mm_set1_epi32(0xffffffff);

__m128i build128(int num, int bitLength, int offset) {
    int r[SIMD_LEN / INT_LEN];

    for (int i = 0; i < SIMD_LEN / INT_LEN; i++) {
        int val = 0;
        int current = offset;
        if (offset != 0 && i != 0) {
            val |= (num >> (bitLength - offset));
        }
        while (current < INT_LEN) {
            val |= (num << current);
            current += bitLength;
        }
        r[i] = val;
        offset = bitLength - ((INT_LEN - offset) % bitLength);
    }
    return _mm_setr_epi32(r[0], r[1], r[2], r[3]);
}

__m128i buildMask128(int bitLength, int offset) {
    return build128(1 << (bitLength - 1), bitLength, offset);
}

int buildPiece128(__m128i prev, __m128i current, int entrySize, int bitOffset) {
    int piece1 = _mm_extract_epi32(prev, 3);
    int piece2 = _mm_extract_epi32(current, 0);
    int s1 = entrySize - bitOffset;
    int num = piece1 >> (INT_LEN - s1) & ((1 << s1) - 1);
    num |= (piece2 << s1) & (((1 << bitOffset) - 1) << s1);
    return num;
}

HaoScanner128::HaoScanner128(int es, bool aligned) {
    this->entrySize = es;
    assert(entrySize < 32 && entrySize > 0);

    this->aligned = aligned;

    int ALIGN = SIMD_LEN / BYTE_LEN;
    int LEN = entrySize * ALIGN;

    this->val1s = (__m128i *) aligned_alloc(ALIGN, LEN);
    this->val2s = (__m128i *) aligned_alloc(ALIGN, LEN);
    this->nval1s = (__m128i *) aligned_alloc(ALIGN, LEN);
    this->nval2s = (__m128i *) aligned_alloc(ALIGN, LEN);
    this->msbmasks = (__m128i *) aligned_alloc(ALIGN, LEN);
    this->notmasks = (__m128i *) aligned_alloc(ALIGN, LEN);
    this->nmval1s = (__m128i *) aligned_alloc(ALIGN, LEN);
    this->nmval2s = (__m128i *) aligned_alloc(ALIGN, LEN);

    for (int i = 0; i < entrySize; i++) {
        this->msbmasks[i] = buildMask128(entrySize, i);
        this->notmasks[i] = _mm_xor_si128(this->msbmasks[i], one);
    }
}

HaoScanner128::~HaoScanner128() {
    free(this->val1s);
    free(this->val2s);
    free(this->nval1s);
    free(this->nval2s);
    free(this->nmval1s);
    free(this->nmval2s);
    free(this->msbmasks);
    free(this->notmasks);
}

void HaoScanner128::scan(int *data, uint64_t length, int *dest, Predicate *p) {
    // XXX For experimental purpose, assume data is aligned for now
    assert(length % (SIMD_LEN / INT_LEN) == 0);

    this->data = data;
    this->dest = dest;
    this->length = length;
    this->predicate = p;

    for (int i = 0; i < entrySize; i++) {
        this->val1s[i] = build128(p->getVal1(), entrySize, i);
        this->val2s[i] = build128(p->getVal2(), entrySize, i);
        this->nval1s[i] = _mm_xor_si128(this->val1s[i], one);
        this->nval2s[i] = _mm_xor_si128(this->val2s[i], one);
        this->nmval1s[i] = _mm_and_si128(this->val1s[i], this->notmasks[i]);
        this->nmval2s[i] = _mm_and_si128(this->val2s[i], this->notmasks[i]);
    }

    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
            if (aligned) {
                alignedEq();
            } else {
                unalignedEq();
            }
            break;
        case opr_in:
            if (aligned) {
                alignedIn();
            } else {
                unalignedIn();
            }
            break;
        default:
            break;
    }
}

void HaoScanner128::alignedEq() {
    __m128i *mdata = (__m128i *) data;
    __m128i *mdest = (__m128i *) dest;

    int bitOffset = 0;

    uint64_t numBit = entrySize * length;
    uint64_t numLane = numBit / SIMD_LEN + ((numBit % SIMD_LEN) ? 1 : 0);

    uint64_t laneCounter = 0;

    __m128i prev;

    while (laneCounter < numLane) {
        __m128i eqnum = this->val1s[bitOffset];
        __m128i notmask = this->notmasks[bitOffset];

        __m128i current = _mm_stream_load_si128(mdata + laneCounter);

        __m128i d = _mm_xor_si128(current, eqnum);
        __m128i result = _mm_or_si128(
                mm_add_epi128(_mm_and_si128(d, notmask), notmask), d);

        if (bitOffset != 0) {
            // Has remain to process
            int num = buildPiece128(prev, current, entrySize, bitOffset);
            __m128i remain = _mm_setr_epi32((num != predicate->getVal1()) << (bitOffset - 1), 0, 0, 0);
            result = _mm_or_si128(result, remain);
        }
        _mm_stream_si128(mdest + (laneCounter++), result);

        bitOffset = entrySize - (SIMD_LEN - bitOffset) % entrySize;

        prev = current;
    }
}

void HaoScanner128::unalignedEq() {
    void *byteData = data;
    void *byteDest = dest;
    int byteOffset = 0;
    int bitOffset = 0;

    uint64_t entryCounter = 0;

    while (entryCounter < length) {
        __m128i eqnum = this->val1s[bitOffset];
        __m128i notmask = this->notmasks[bitOffset];

        __m128i current = _mm_loadu_si128((__m128i *) (byteData + byteOffset));
        __m128i d = _mm_xor_si128(current, eqnum);
        __m128i result = _mm_or_si128(
                mm_add_epi128(_mm_and_si128(d, notmask), notmask), d);
        __m128i resmask = _mm_setr_epi32(-1 << bitOffset, -1, -1, -1);
        int existMask = (1 << bitOffset) - 1;
        int exist = (int) *((char *) (byteDest + byteOffset));

        __m128i joined = _mm_xor_si128(_mm_and_si128(resmask, result),
                                       _mm_setr_epi32(existMask & exist, 0, 0, 0));

        _mm_storeu_si128((__m128i *) (byteDest + byteOffset), joined);

        int numFullEntry = (SIMD_LEN - bitOffset) / entrySize;
        entryCounter += numFullEntry;
        int bitAdvance = (bitOffset + numFullEntry * entrySize);
        int byteAdvance = bitAdvance / BYTE_LEN;
        byteOffset += byteAdvance;
        bitOffset = bitAdvance % BYTE_LEN;
    }
}

void HaoScanner128::alignedIn() {
    __m128i *mdata = (__m128i *) data;
    __m128i *mdest = (__m128i *) dest;

    int bitOffset = 0;

    uint64_t numBit = entrySize * length;
    uint64_t numLane = numBit / SIMD_LEN + (numBit % SIMD_LEN ? 1 : 0);

    uint64_t laneCounter = 0;

    __m128i prev;

    while (laneCounter < numLane) {
        __m128i mask = this->msbmasks[bitOffset];
        __m128i aornm = this->nmval1s[bitOffset];
        __m128i bornm = this->nmval2s[bitOffset];
        __m128i na = this->nval1s[bitOffset];
        __m128i nb = this->nval2s[bitOffset];

        __m128i current = _mm_stream_load_si128(mdata + laneCounter);

        __m128i xorm = _mm_or_si128(current, mask);
        __m128i l = mm_sub_epi128(xorm, aornm);
        __m128i h = mm_sub_epi128(xorm, bornm);
        __m128i el = _mm_and_si128(_mm_or_si128(current, na),
                                   _mm_or_si128(_mm_and_si128(current, na), l));
        __m128i eh = _mm_and_si128(_mm_or_si128(current, nb),
                                   _mm_or_si128(_mm_and_si128(current, nb), h));
        __m128i result = _mm_xor_si128(el, eh);
        if (bitOffset != 0) {
            // Has remain to process
            int num = buildPiece128(prev, current, entrySize, bitOffset);
            __m128i remain = _mm_setr_epi32(
                    (num >= predicate->getVal1() && num < predicate->getVal2()) << (bitOffset - 1), 0, 0, 0);
            result = _mm_or_si128(result, remain);
        }
        _mm_stream_si128(mdest + (laneCounter++), result);

        bitOffset = entrySize - (SIMD_LEN - bitOffset) % entrySize;

        prev = current;
    }
}

void HaoScanner128::unalignedIn() {
    void *byteData = data;
    void *byteDest = dest;
    int byteOffset = 0;
    int bitOffset = 0;

    uint64_t entryCounter = 0;

    while (entryCounter < length) {
        __m128i mask = this->msbmasks[bitOffset];
        __m128i aornm = this->nmval1s[bitOffset];
        __m128i bornm = this->nmval2s[bitOffset];
        __m128i na = this->nval1s[bitOffset];
        __m128i nb = this->nval2s[bitOffset];

        __m128i current = _mm_loadu_si128((__m128i *) (byteData + byteOffset));
        __m128i xorm = _mm_or_si128(current, mask);
        __m128i l = mm_sub_epi128(xorm, aornm);
        __m128i h = mm_sub_epi128(xorm, bornm);
        __m128i el = _mm_and_si128(_mm_or_si128(current, na),
                                   _mm_or_si128(_mm_and_si128(current, na), l));
        __m128i eh = _mm_and_si128(_mm_or_si128(current, nb),
                                   _mm_or_si128(_mm_and_si128(current, nb), h));
        __m128i result = _mm_xor_si128(el, eh);
        __m128i resmask = _mm_setr_epi32(-1 << bitOffset, -1, -1, -1);
        int existMask = (1 << bitOffset) - 1;
        int exist = (int) *((char *) (byteDest + byteOffset));

        __m128i joined = _mm_xor_si128(_mm_and_si128(resmask, result),
                                       _mm_setr_epi32(existMask & exist, 0, 0, 0));

        _mm_storeu_si128((__m128i *) (byteDest + byteOffset), joined);

        int numFullEntry = (SIMD_LEN - bitOffset) / entrySize;
        entryCounter += numFullEntry;
        int bitAdvance = (bitOffset + numFullEntry * entrySize);
        int byteAdvance = bitAdvance / BYTE_LEN;
        byteOffset += byteAdvance;
        bitOffset = bitAdvance % BYTE_LEN;
    }
}

