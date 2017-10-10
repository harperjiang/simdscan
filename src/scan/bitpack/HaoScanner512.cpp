//
// Created by harper on 9/19/17.
//

#include <cassert>
#include "HaoScanner512.h"
#include "../../util/math_util.h"

#define INT_LEN 32
#define SIMD_LEN 512
#define BYTE_LEN 8

const __m512i one = _mm512_set1_epi32(0xffffffff);

__m512i build512(int num, int bitLength, int offset) {
    int r[16];

    for (int i = 0; i < 16; i++) {
        int val = 0;
        int current = offset;
        if (offset != 0) {
            val |= (num >> (bitLength - offset));
        }
        while (current < INT_LEN) {
            val |= (num << current);
            current += bitLength;
        }
        r[i] = val;
        offset = bitLength - (INT_LEN - offset) % bitLength;
    }
    return _mm512_setr_epi32(r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8], r[9], r[10], r[11], r[12], r[13],
                             r[14], r[15]);
}

__m512i buildMask512(int bitLength, int offset) {
    return build512(1 << (bitLength - 1), bitLength, offset);
}

int buildPiece512(__m512i prev, __m512i current, int entrySize, int bitOffset) {
    int piece1 = _mm_extract_epi32(_mm512_extracti32x4_epi32(prev, 3), 3);
    int piece2 = _mm_extract_epi32(_mm512_extracti32x4_epi32(current, 0), 0);
    int s1 = entrySize - bitOffset;
    int num = piece1 >> (INT_LEN - s1) & ((1 << s1) - 1);
    num |= (piece2 << s1) & (((1 << bitOffset) - 1) << s1);
    return num;
}

HaoScanner512::HaoScanner512(int es, bool aligned) {
    assert(es < 32 && es > 0);
    this->entrySize = es;

    this->aligned = aligned;


    int ALIGN = SIMD_LEN / BYTE_LEN;
    int LEN = entrySize * ALIGN;


    this->val1s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->val2s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->nval1s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->nval2s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->msbmasks = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->notmasks = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->nmval1s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->nmval2s = (__m512i *) aligned_alloc(ALIGN, LEN);

    for (int i = 0; i < entrySize; i++) {
        this->msbmasks[i] = buildMask512(entrySize, i);
        this->notmasks[i] = _mm512_xor_si512(this->msbmasks[i], one);
    }
}

HaoScanner512::~HaoScanner512() {
    free(this->val1s);
    free(this->val2s);
    free(this->nval1s);
    free(this->nval2s);
    free(this->nmval1s);
    free(this->nmval2s);
    free(this->msbmasks);
    free(this->notmasks);
}

void HaoScanner512::scan(int *data, uint64_t length, int *dest, Predicate *p) {
    this->data = data;
    this->dest = dest;
    this->length = length;
    this->predicate = p;

    for (int i = 0; i < entrySize; i++) {
        this->val1s[i] = build512(p->getVal1(), entrySize, i);
        this->val2s[i] = build512(p->getVal2(), entrySize, i);
        this->nval1s[i] = _mm512_xor_si512(this->val1s[i], one);
        this->nval2s[i] = _mm512_xor_si512(this->val2s[i], one);
        this->nmval1s[i] = _mm512_and_si512(this->val1s[i], this->notmasks[i]);
        this->nmval2s[i] = _mm512_and_si512(this->val2s[i], this->notmasks[i]);
    }

    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
            if (aligned)
                alignedEq();
            else
                unalignedEq();
            break;
        case opr_in:
            if (aligned)
                alignedIn();
            else
                unalignedIn();
            break;
        default:
            break;
    }
}

void HaoScanner512::alignedEq() {
    __m512i *mdata = (__m512i *) data;
    __m512i *mdest = (__m512i *) dest;

    int bitOffset = 0;

    uint64_t numBit = entrySize * length;
    uint64_t numLane = numBit / SIMD_LEN + ((numBit % SIMD_LEN) ? 1 : 0);

    uint64_t laneCounter = 0;

    __m512i prev;

    while (laneCounter < numLane) {
        __m512i eqnum = this->val1s[bitOffset];
        __m512i notmask = this->notmasks[bitOffset];

        __m512i current = _mm512_stream_load_si512(mdata + laneCounter);

        __m512i d = _mm512_xor_si512(current, eqnum);
        __m512i result = _mm512_or_si512(
                mm512_add_epi512(_mm512_and_si512(d, notmask), notmask), d);

        if (bitOffset != 0) {
            // Has remain to process
            int num = buildPiece512(prev, current, entrySize, bitOffset);
            __m512i remain = _mm512_setr_epi64((num != predicate->getVal1()) << (bitOffset - 1), 0, 0, 0, 0, 0, 0, 0);
            result = _mm512_or_si512(result, remain);
        }
        _mm512_stream_si512(mdest + (laneCounter++), result);

        bitOffset = entrySize - (SIMD_LEN - bitOffset) % entrySize;

        prev = current;
    }
}

void HaoScanner512::alignedIn() {
    __m512i *mdata = (__m512i *) data;
    __m512i *mdest = (__m512i *) dest;

    int bitOffset = 0;

    uint64_t numBit = entrySize * length;
    uint64_t numLane = numBit / SIMD_LEN + (numBit % SIMD_LEN ? 1 : 0);

    uint64_t laneCounter = 0;

    __m512i prev;

    while (laneCounter < numLane) {
        __m512i mask = this->msbmasks[bitOffset];
        __m512i aornm = this->nmval1s[bitOffset];
        __m512i bornm = this->nmval2s[bitOffset];
        __m512i na = this->nval1s[bitOffset];
        __m512i nb = this->nval2s[bitOffset];

        __m512i current = _mm512_stream_load_si512(mdata + laneCounter);

        __m512i xorm = _mm512_or_si512(current, mask);
        __m512i l = mm512_sub_epi512(xorm, aornm);
        __m512i h = mm512_sub_epi512(xorm, bornm);
        __m512i el = _mm512_and_si512(_mm512_or_si512(current, na),
                                      _mm512_or_si512(_mm512_and_si512(current, na), l));
        __m512i eh = _mm512_and_si512(_mm512_or_si512(current, nb),
                                      _mm512_or_si512(_mm512_and_si512(current, nb), h));
        __m512i result = _mm512_xor_si512(el, eh);
        if (bitOffset != 0) {
            // Has remain to process
            int num = buildPiece512(prev, current, entrySize, bitOffset);
            __m512i remain = _mm512_setr_epi64(
                    (num >= predicate->getVal1() && num < predicate->getVal2()) << (bitOffset - 1), 0, 0, 0, 0, 0, 0,
                    0);
            result = _mm512_or_si512(result, remain);
        }
        _mm512_stream_si512(mdest + (laneCounter++), result);

        bitOffset = entrySize - (SIMD_LEN - bitOffset) % entrySize;

        prev = current;
    }
}

void HaoScanner512::unalignedEq() {
    void *byteData = data;
    void *byteDest = dest;
    int byteOffset = 0;
    int bitOffset = 0;

    uint64_t entryCounter = 0;

    while (entryCounter < length) {
        __m512i eqnum = this->val1s[bitOffset];
        __m512i notmask = this->notmasks[bitOffset];

        __m512i current = _mm512_loadu_si512(
                (__m512i *) (byteData + byteOffset));
        __m512i d = _mm512_xor_si512(current, eqnum);
        __m512i result = _mm512_or_si512(
                mm512_add_epi512(_mm512_and_si512(d, notmask), notmask), d);
        __m512i resmask = _mm512_setr_epi64(-1 << bitOffset, -1, -1, -1, -1, -1, -1, -1);
        int existMask = (1 << bitOffset) - 1;
        int exist = (int) *((char *) (byteDest + byteOffset));

        __m512i joined = _mm512_xor_si512(_mm512_and_si512(resmask, result),
                                          _mm512_setr_epi64(existMask & exist, 0, 0, 0, 0, 0, 0, 0));

        _mm512_storeu_si512((__m512i *) (byteDest + byteOffset), joined);

        int numFullEntry = (SIMD_LEN - bitOffset) / entrySize;
        entryCounter += numFullEntry;
        int bitAdvance = (bitOffset + numFullEntry * entrySize);
        int byteAdvance = bitAdvance / BYTE_LEN;
        byteOffset += byteAdvance;
        bitOffset = bitAdvance % BYTE_LEN;
    }
}

void HaoScanner512::unalignedIn() {

    void *byteData = data;
    void *byteDest = dest;
    int byteOffset = 0;
    int bitOffset = 0;

    uint64_t entryCounter = 0;

    while (entryCounter < length) {
        __m512i mask = this->msbmasks[bitOffset];
        __m512i aornm = this->nmval1s[bitOffset];
        __m512i bornm = this->nmval2s[bitOffset];
        __m512i na = this->nval1s[bitOffset];
        __m512i nb = this->nval2s[bitOffset];

        __m512i current = _mm512_loadu_si512(
                (__m512i *) (byteData + byteOffset));
        __m512i xorm = _mm512_or_si512(current, mask);
        __m512i l = mm512_sub_epi512(xorm, aornm);
        __m512i h = mm512_sub_epi512(xorm, bornm);
        __m512i el = _mm512_and_si512(_mm512_or_si512(current, na),
                                      _mm512_or_si512(_mm512_and_si512(current, na), l));
        __m512i eh = _mm512_and_si512(_mm512_or_si512(current, nb),
                                      _mm512_or_si512(_mm512_and_si512(current, nb), h));
        __m512i result = _mm512_xor_si512(el, eh);

        __m512i resmask = _mm512_setr_epi64(-1 << bitOffset, -1, -1, -1, -1, -1, -1, -1);
        int existMask = (1 << bitOffset) - 1;
        int exist = (int) *((char *) (byteDest + byteOffset));

        __m512i joined = _mm512_xor_si512(_mm512_and_si512(resmask, result),
                                          _mm512_setr_epi64(existMask & exist, 0, 0, 0, 0, 0, 0, 0));

        _mm512_storeu_si512((__m512i *) (byteDest + byteOffset), joined);

        int numFullEntry = (SIMD_LEN - bitOffset) / entrySize;
        entryCounter += numFullEntry;
        int bitAdvance = (bitOffset + numFullEntry * entrySize);
        int byteAdvance = bitAdvance / BYTE_LEN;
        byteOffset += byteAdvance;
        bitOffset = bitAdvance % BYTE_LEN;
    }

}
