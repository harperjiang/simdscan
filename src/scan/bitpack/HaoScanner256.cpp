/*
 * HaoScanner256.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include <immintrin.h>
#include <assert.h>
#include "HaoScanner256.h"
#include "../../util/math_util.h"

#define INT_LEN 32
#define SIMD_LEN 256
#define BYTE_LEN 8
#define BYTE_IN_SIMD 32

const __m256i one = _mm256_set1_epi32(0xffffffff);

__m256i build(int num, int bitLength, int offset) {
    int r[8];

    for (int i = 0; i < 8; i++) {
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
        offset = bitLength - (INT_LEN - offset) % bitLength;
    }
    return _mm256_setr_epi32(r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
}

__m256i buildMask(int bitLength, int offset) {
    return build(1 << (bitLength - 1), bitLength, offset);
}

int buildPiece(__m256i prev, __m256i current, int entrySize, int bitOffset) {
    int piece1 = _mm256_extract_epi32(prev, 7);
    int piece2 = _mm256_extract_epi32(current, 0);
    int s1 = entrySize - bitOffset;
    int num = piece1 >> (INT_LEN - s1) & ((1 << s1) - 1);
    num |= (piece2 << s1) & (((1 << bitOffset) - 1) << s1);
    return num;
}

HaoScanner256::HaoScanner256(int es, bool aligned) {
    assert(es < 32 && es > 0);
    this->entrySize = es;

    this->aligned = aligned;


    int ALIGN = SIMD_LEN / BYTE_LEN;
    int LEN = entrySize * ALIGN;


    this->val1s = (__m256i *) aligned_alloc(ALIGN, LEN);
    this->val2s = (__m256i *) aligned_alloc(ALIGN, LEN);
    this->nval1s = (__m256i *) aligned_alloc(ALIGN, LEN);
    this->nval2s = (__m256i *) aligned_alloc(ALIGN, LEN);
    this->msbmasks = (__m256i *) aligned_alloc(ALIGN, LEN);
    this->notmasks = (__m256i *) aligned_alloc(ALIGN, LEN);
    this->nmval1s = (__m256i *) aligned_alloc(ALIGN, LEN);
    this->nmval2s = (__m256i *) aligned_alloc(ALIGN, LEN);

    for (int i = 0; i < entrySize; i++) {
        this->msbmasks[i] = buildMask(entrySize, i);
        this->notmasks[i] = _mm256_xor_si256(this->msbmasks[i], one);
    }
}

HaoScanner256::~HaoScanner256() {
    free(this->val1s);
    free(this->val2s);
    free(this->nval1s);
    free(this->nval2s);
    free(this->nmval1s);
    free(this->nmval2s);
    free(this->msbmasks);
    free(this->notmasks);
}

void HaoScanner256::scan(int *data, uint64_t length, int *dest, Predicate *p) {
    this->data = data;
    this->dest = dest;
    this->length = length;
    this->predicate = p;

    for (int i = 0; i < entrySize; i++) {
        this->val1s[i] = build(p->getVal1(), entrySize, i);
        this->val2s[i] = build(p->getVal2(), entrySize, i);
        this->nval1s[i] = _mm256_xor_si256(this->val1s[i], one);
        this->nval2s[i] = _mm256_xor_si256(this->val2s[i], one);
        this->nmval1s[i] = _mm256_and_si256(this->val1s[i], this->notmasks[i]);
        this->nmval2s[i] = _mm256_and_si256(this->val2s[i], this->notmasks[i]);
    }

    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
            if (aligned)
                alignedEq();
            else
                unalignedEq();
            break;
        case opr_less:
            if (aligned)
                alignedLess();
            else
                unalignedLess();
            break;
        default:
            break;
    }
}

void HaoScanner256::alignedEq() {
    __m256i *mdata = (__m256i *) data;
    __m256i *mdest = (__m256i *) dest;
    uint8_t *bytedest = (uint8_t *) dest;

    int bitOffset = 0;

    uint64_t numBit = entrySize * length;
    uint64_t numLane = numBit / SIMD_LEN + ((numBit % SIMD_LEN) ? 1 : 0);

    uint64_t laneCounter = 0;

    __m256i prev;

    while (laneCounter < numLane) {
        __m256i eqnum = this->val1s[bitOffset];
        __m256i notmask = this->notmasks[bitOffset];

        __m256i current = _mm256_stream_load_si256(mdata + laneCounter);

        __m256i d = _mm256_xor_si256(current, eqnum);
        __m256i result = _mm256_or_si256(
                mm256_add_epi256(_mm256_and_si256(d, notmask), notmask), d);

        _mm256_stream_si256(mdest + (laneCounter++), result);
        if (bitOffset != 0) {
            // Has remain to process
            int num = buildPiece(prev, current, entrySize, bitOffset);

            int remainIdx = bitOffset / 8;
            int remainOffset = bitOffset % 8;
            uint32_t remain = (num != predicate->getVal1()) << (remainOffset - 1);
            uint8_t set = bytedest[(laneCounter - 1) * BYTE_IN_SIMD + remainIdx];
            set &= invmasks[remainOffset];
            set |= remain;
            bytedest[(laneCounter - 1) * BYTE_IN_SIMD + remainIdx] = set;
        }

        bitOffset = entrySize - (SIMD_LEN - bitOffset) % entrySize;

        prev = current;
    }
}

void HaoScanner256::alignedLess() {
    __m256i *mdata = (__m256i *) data;
    __m256i *mdest = (__m256i *) dest;
    uint8_t *bytedest = (uint8_t *) dest;

    int bitOffset = 0;

    uint64_t numBit = entrySize * length;
    uint64_t numLane = numBit / SIMD_LEN + (numBit % SIMD_LEN ? 1 : 0);

    uint64_t laneCounter = 0;

    __m256i prev;

    while (laneCounter < numLane) {
        __m256i mask = this->msbmasks[bitOffset];
        __m256i aornm = this->nmval1s[bitOffset];
        __m256i na = this->nval1s[bitOffset];

        __m256i current = _mm256_stream_load_si256(mdata + laneCounter);

        __m256i xorm = _mm256_or_si256(current, mask);
        __m256i l = mm256_sub_epi256(xorm, aornm);
        __m256i result = _mm256_and_si256(_mm256_or_si256(current, na),
                                          _mm256_or_si256(_mm256_and_si256(current, na), l));

        _mm256_stream_si256(mdest + (laneCounter++), result);
        if (bitOffset != 0) {
            // Has remain to process
            int num = buildPiece(prev, current, entrySize, bitOffset);

            int remainIdx = bitOffset / 8;
            int remainOffset = bitOffset % 8;
            uint32_t remain = (num >= predicate->getVal1()) << (remainOffset - 1);
            uint8_t set = bytedest[(laneCounter - 1) * BYTE_IN_SIMD + remainIdx];
            set &= invmasks[remainOffset];
            set |= remain;
            bytedest[(laneCounter - 1) * BYTE_IN_SIMD + remainIdx] = set;
        }

        bitOffset = entrySize - (SIMD_LEN - bitOffset) % entrySize;

        prev = current;
    }
}

void HaoScanner256::unalignedEq() {
    void *byteData = data;
    void *byteDest = dest;
    int byteOffset = 0;
    int bitOffset = 0;

    uint64_t entryCounter = 0;

    while (entryCounter < length) {
        __m256i eqnum = this->val1s[bitOffset];
        __m256i notmask = this->notmasks[bitOffset];

        __m256i current = _mm256_loadu_si256(
                (__m256i *) (byteData + byteOffset));
        __m256i d = _mm256_xor_si256(current, eqnum);
        __m256i result = _mm256_or_si256(
                mm256_add_epi256(_mm256_and_si256(d, notmask), notmask), d);

        uint8_t preserve = ((uint8_t *) byteDest)[byteOffset];
        preserve &= masks[bitOffset];
        _mm256_storeu_si256((__m256i *) (byteDest + byteOffset), result);
        ((uint8_t *) byteDest)[byteOffset] |= preserve;

        entryCounter += (SIMD_LEN - bitOffset) / entrySize;

        int partialEntryLen = (SIMD_LEN - bitOffset) % entrySize;
        int partialBytes = (partialEntryLen / 8) + ((partialEntryLen % 8) ? 1 : 0);
        byteOffset += BYTE_IN_SIMD - partialBytes;
        bitOffset = partialBytes * 8 - partialEntryLen;
    }
}

void HaoScanner256::unalignedLess() {

    void *byteData = data;
    void *byteDest = dest;
    int byteOffset = 0;
    int bitOffset = 0;

    uint64_t entryCounter = 0;

    while (entryCounter < length) {
        __m256i mask = this->msbmasks[bitOffset];
        __m256i aornm = this->nmval1s[bitOffset];
        __m256i na = this->nval1s[bitOffset];

        __m256i current = _mm256_loadu_si256(
                (__m256i *) (byteData + byteOffset));
        __m256i xorm = _mm256_or_si256(current, mask);
        __m256i l = mm256_sub_epi256(xorm, aornm);
        __m256i result = _mm256_and_si256(_mm256_or_si256(current, na),
                                      _mm256_or_si256(_mm256_and_si256(current, na), l));

        uint8_t preserve = ((uint8_t *) byteDest)[byteOffset];
        preserve &= masks[bitOffset];
        _mm256_storeu_si256((__m256i *) (byteDest + byteOffset), result);
        ((uint8_t *) byteDest)[byteOffset] |= preserve;

        entryCounter += (SIMD_LEN - bitOffset) / entrySize;

        int partialEntryLen = (SIMD_LEN - bitOffset) % entrySize;
        int partialBytes = (partialEntryLen / 8) + ((partialEntryLen % 8) ? 1 : 0);
        byteOffset += BYTE_IN_SIMD - partialBytes;
        bitOffset = partialBytes * 8 - partialEntryLen;
    }

}

