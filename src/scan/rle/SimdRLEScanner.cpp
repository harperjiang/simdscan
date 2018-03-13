//
// Created by harper on 2/6/18.
//

#include "SimdRLEScanner.h"
#include "../../util/math_util.h"

#define SIMD_LEN 512
#define INT_LEN 32
#define BYTE_LEN 8

const __m512i one = _mm512_set1_epi32(0xffffffff);

extern __m512i build512(int num, int bitLength, int offset);

__m512i buildMask(int es, int rls, int offset) {
    return build512(1 << (es - 1), es + rls, offset);
}

int buildPiece(__m512i prev, __m512i current, int entrySize, int bitOffset) {
    int piece1 = _mm_extract_epi32(_mm512_extracti32x4_epi32(prev, 3), 3);
    int piece2 = _mm_extract_epi32(_mm512_extracti32x4_epi32(current, 0), 0);
    int s1 = entrySize - bitOffset;
    int num = piece1 >> (INT_LEN - s1) & ((1 << s1) - 1);
    num |= (piece2 << s1) & (((1 << bitOffset) - 1) << s1);
    return num;
}

SimdRLEScanner::SimdRLEScanner(int es, int rls) {
    this->entrySize = es;
    this->rlSize = rls;
    this->groupSize = entrySize + rlSize;

    int ALIGN = SIMD_LEN / BYTE_LEN;
    int LEN = (entrySize + rlSize) * ALIGN;


    this->val1s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->val2s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->nval1s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->nval2s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->msbmasks = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->notmasks = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->nmval1s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->nmval2s = (__m512i *) aligned_alloc(ALIGN, LEN);
    this->rlmasks = (__m512i *) aligned_alloc(ALIGN, LEN);
    for (int i = 0; i < entrySize; i++) {
        this->msbmasks[i] = build512(1 << (entrySize - 1), groupSize, i);
        this->notmasks[i] = build512((1 << (entrySize - 1)) - 1, groupSize, i);
        this->rlmasks[i] = build512((1 << entrySize) - 1, groupSize, i);
    }
}

SimdRLEScanner::~SimdRLEScanner() {

}

void SimdRLEScanner::scan(int *input, uint64_t size, int *output, Predicate *p) {

    this->input = (__m512i *) input;
    this->output = (__m512i *) output;
    this->numEntry = size;
    this->predicate = p;

    int entryMask = (1 << (entrySize)) - 1;
    int rbMask = (1 << (entrySize - 1)) - 1;
    int nval1 = p->getVal1() ^entryMask;
    int nval2 = p->getVal2() ^entryMask;
    int nmval1 = p->getVal1() & rbMask;
    int nmval2 = p->getVal2() & rbMask;

    for (int i = 0; i < entrySize; i++) {
        this->val1s[i] = build512(p->getVal1(), groupSize, i);
        this->val2s[i] = build512(p->getVal2(), groupSize, i);
        this->nval1s[i] = build512(nval1, groupSize, i);
        this->nval2s[i] = build512(nval2, groupSize, i);
        this->nmval1s[i] = build512(nmval1, groupSize, i);
        this->nmval2s[i] = build512(nmval2, groupSize, i);
    }

    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
            equal();
            break;
        case opr_in:
            range();
            break;
        default:
            break;
    }
}

void SimdRLEScanner::equal() {
    int bitOffset = 0;
    int entryOffset = 0;

    uint64_t numBit = groupSize * numEntry;
    uint64_t numLane = numBit / SIMD_LEN + ((numBit % SIMD_LEN) ? 1 : 0);

    uint64_t laneCounter = 0;

    __m512i prev;

    while (laneCounter < numLane) {
        __m512i eqnum = this->val1s[bitOffset];
        __m512i notmask = this->notmasks[bitOffset];

        __m512i current = _mm512_stream_load_si512(input + laneCounter);

        __m512i d = _mm512_xor_si512(current, eqnum);
        __m512i result = _mm512_or_si512(
                mm512_add_epi512(_mm512_and_si512(d, notmask), notmask), d);

        if (bitOffset > rlSize) {
            // When bitOffset <= rlSize, it is the run-length section at boundary and no need to process
            // Has remain to process
            int num = buildPiece(prev, current, entrySize, bitOffset - rlSize);
            __m512i remain = _mm512_setr_epi64((num != predicate->getVal1()) << (bitOffset - 1), 0, 0, 0, 0, 0, 0, 0);
            result = _mm512_or_si512(result, remain);
        }
        _mm512_stream_si512(output + (laneCounter++), result);

        bitOffset = groupSize - (SIMD_LEN - bitOffset) % groupSize;
        prev = current;
    }
}

void SimdRLEScanner::range() {

    int bitOffset = 0;

    uint64_t numBit = entrySize * numEntry;
    uint64_t numLane = numBit / SIMD_LEN + (numBit % SIMD_LEN ? 1 : 0);

    uint64_t laneCounter = 0;

    __m512i prev;

    while (laneCounter < numLane) {
        __m512i mask = this->msbmasks[bitOffset];
        __m512i aornm = this->nmval1s[bitOffset];
        __m512i bornm = this->nmval2s[bitOffset];
        __m512i na = this->nval1s[bitOffset];
        __m512i nb = this->nval2s[bitOffset];

        __m512i rlmask = this->rlmasks[bitOffset];

        __m512i current = _mm512_stream_load_si512(input + laneCounter);

        __m512i xorm = _mm512_or_si512(current, mask);
        __m512i l = mm512_sub_epi512(xorm, aornm);
        __m512i h = mm512_sub_epi512(xorm, bornm);
        __m512i el = _mm512_and_si512(_mm512_or_si512(current, na),
                                      _mm512_or_si512(_mm512_and_si512(current, na), l));
        __m512i eh = _mm512_and_si512(_mm512_or_si512(current, nb),
                                      _mm512_or_si512(_mm512_and_si512(current, nb), h));

        __m512i result = _mm512_xor_si512(el, _mm512_and_si512(eh, rlmask));

        if (bitOffset > rlSize) {
            // Has remain to process
            int num = buildPiece(prev, current, entrySize, bitOffset - rlSize);
            __m512i remain = _mm512_setr_epi64(
                    (num >= predicate->getVal1() && num < predicate->getVal2()) << (bitOffset - 1), 0, 0, 0, 0, 0, 0,
                    0);
            result = _mm512_or_si512(result, remain);
        }
        _mm512_stream_si512(output + (laneCounter++), result);

        bitOffset = groupSize - (SIMD_LEN - bitOffset) % groupSize;

        prev = current;
    }
}