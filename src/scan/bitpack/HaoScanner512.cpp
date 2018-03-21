//
// Created by harper on 9/19/17.
//

#include <cassert>
#include <stdio.h>
#include "HaoScanner512.h"
#include "../../util/math_util.h"

#define INT_LEN 32
#define SIMD_LEN 512
#define BYTE_LEN 8
#define BYTE_IN_SIMD 64

const __m512i one = _mm512_set1_epi32(0xffffffff);

__m512i build512(int num, int bitLength, int offset) {
    int r[16];

    for (int i = 0; i < 16; i++) {
        int val = 0;
        int current = offset;
        // Leave bits before offset to 0 can preserve result in last entry.
        // This is useful for unaligned load
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
    return _mm512_setr_epi32(r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8], r[9], r[10], r[11], r[12], r[13],
                             r[14], r[15]);
}

__m512i buildMask512(int bitLength, int offset) {
    return build512(1 << (bitLength - 1), bitLength, offset);
}

HaoScanner512::HaoScanner512(int es) {
    assert(es < 32 && es > 0);
    this->entrySize = es;

    int ALIGN = SIMD_LEN / BYTE_LEN;
    int LEN = 8 * ALIGN;


    this->val1s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->nval1s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->msbmasks = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->notmasks = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->nmval1s = (__m512i *) aligned_alloc(ALIGN, LEN);

    for (int i = 0; i < 8; i++) {
        this->msbmasks[i] = buildMask512(entrySize, i);
        this->notmasks[i] = _mm512_xor_si512(this->msbmasks[i], one);
    }
}

HaoScanner512::~HaoScanner512() {
    free(this->val1s);
    free(this->nval1s);
    free(this->nmval1s);
    free(this->msbmasks);
    free(this->notmasks);
}

void HaoScanner512::scan(int *data, uint64_t length, int *dest, Predicate *p) {
    this->data = data;
    this->dest = dest;
    this->length = length;
    this->predicate = p;

    for (int i = 0; i < 8; i++) {
        this->val1s[i] = build512(p->getVal1(), entrySize, i);
        this->nval1s[i] = _mm512_xor_si512(this->val1s[i], one);
        this->nmval1s[i] = _mm512_and_si512(this->val1s[i], this->notmasks[i]);
    }

    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
            equal();
            break;
        case opr_less:
            less();
            break;
        default:
            break;
    }
}

void HaoScanner512::equal() {
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

        uint8_t preserve = ((uint8_t *) byteDest)[byteOffset];
        preserve &= masks[bitOffset];
        _mm512_storeu_si512((__m512i *) (byteDest + byteOffset), result);
        ((uint8_t *) byteDest)[byteOffset] |= preserve;

        entryCounter += (SIMD_LEN - bitOffset) / entrySize;

        int partialEntryLen = (SIMD_LEN - bitOffset) % entrySize;
        int partialBytes = (partialEntryLen / 8) + ((partialEntryLen % 8) ? 1 : 0);
        byteOffset += BYTE_IN_SIMD - partialBytes;
        bitOffset = partialBytes * 8 - partialEntryLen;
    }
}

void HaoScanner512::less() {

    void *byteData = data;
    void *byteDest = dest;
    int byteOffset = 0;
    int bitOffset = 0;

    uint64_t entryCounter = 0;

    while (entryCounter < length) {
        __m512i mask = this->msbmasks[bitOffset];
        __m512i aornm = this->nmval1s[bitOffset];
        __m512i na = this->nval1s[bitOffset];

        __m512i current = _mm512_loadu_si512(
                (__m512i *) (byteData + byteOffset));
        __m512i xorm = _mm512_or_si512(current, mask);
        __m512i l = mm512_sub_epi512(xorm, aornm);
        __m512i result = _mm512_and_si512(_mm512_or_si512(current, na),
                                          _mm512_or_si512(_mm512_and_si512(current, na), l));

        _mm512_storeu_si512((__m512i *) (byteDest + byteOffset), result);

        uint8_t preserve = ((uint8_t *) byteDest)[byteOffset];
        preserve &= masks[bitOffset];
        _mm512_storeu_si512((__m512i *) (byteDest + byteOffset), result);
        ((uint8_t *) byteDest)[byteOffset] |= preserve;

        entryCounter += (SIMD_LEN - bitOffset) / entrySize;

        int partialEntryLen = (SIMD_LEN - bitOffset) % entrySize;
        int partialBytes = (partialEntryLen / 8) + ((partialEntryLen % 8) ? 1 : 0);
        byteOffset += BYTE_IN_SIMD - partialBytes;
        bitOffset = partialBytes * 8 - partialEntryLen;

    }
}
