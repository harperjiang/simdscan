//
// Created by harper on 2/6/18.
//

#include "SimdRLEScanner.h"
#include "../../util/math_util.h"

#define SIMD_LEN 512
#define INT_LEN 32
#define BYTE_LEN 8
#define BYTE_IN_SIMD 64

const __m512i one = _mm512_set1_epi32(0xffffffff);

extern __m512i build512(int num, int bitLength, int offset);

int buildRlePiece(__m512i prev, __m512i current, int entrySize, int bitOffset) {
    int piece1 = _mm_extract_epi32(_mm512_extracti32x4_epi32(prev, 3), 3);
    int piece2 = _mm_extract_epi32(_mm512_extracti32x4_epi32(current, 0), 0);
    int s1 = entrySize - bitOffset;
    int num = piece1 >> (INT_LEN - s1) & ((1 << s1) - 1);
    num |= (piece2 << s1) & (((1 << bitOffset) - 1) << s1);
    return num;
}

SimdRLEScanner::SimdRLEScanner(int es, int rls, bool aligned) {
    this->entrySize = es;
    this->rlSize = rls;
    this->aligned = aligned;
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
            if (aligned) {
                    aequal();
            } else {
                if (rlSize >= 8) {
                    equalFast();
                } else {
                    equalNormal();
                }
            }
            break;
        case opr_less:
            if (aligned)
                    aless();
            else {
                if (rlSize >= 8) {
                    lessFast();
                } else {
                    lessNormal();
                }
            }
            break;
        default:
            break;
    }
}

void SimdRLEScanner::aequal() {
    uint8_t *bytedest = (uint8_t *) output;

    int bitOffset = 0;

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

        _mm512_stream_si512(output + (laneCounter++), result);
        if (bitOffset > rlSize) {
            // Has remain to process
            int entryOffset = bitOffset - rlSize;

            int num = buildRlePiece(prev, current, entrySize, entryOffset);

            int remainIdx = (entryOffset) / 8;
            int remainOffset = (entryOffset) % 8;
            uint32_t remain = (num != predicate->getVal1()) << (remainOffset - 1);
            uint8_t set = bytedest[(laneCounter - 1) * BYTE_IN_SIMD + remainIdx];
            set &= invmasks[remainOffset];
            set |= remain;
            bytedest[(laneCounter - 1) * BYTE_IN_SIMD + remainIdx] = set;
        }

        bitOffset = groupSize - (SIMD_LEN - bitOffset) % groupSize;
        prev = current;
    }
}


void SimdRLEScanner::equalFast() {
    uint8_t *uinput = (uint8_t *) input;
    uint8_t *uoutput = (uint8_t *) output;

    uint64_t entryDone = 0;

    uint64_t byteOffset = 0;
    int bitOffset = 0;

    while (entryDone < this->numEntry) {
        __m512i current = _mm512_loadu_si512(uinput + byteOffset);

        __m512i eqnum = this->val1s[bitOffset];
        __m512i notmask = this->notmasks[bitOffset];

        __m512i d = _mm512_xor_si512(current, eqnum);
        __m512i result = _mm512_or_si512(
                mm512_add_epi512(_mm512_and_si512(d, notmask), notmask), d);

        _mm512_storeu_si512((__m512i *) (uoutput + byteOffset), result);

        entryDone += (SIMD_LEN - bitOffset) / groupSize;

        int partialEntryLen = (SIMD_LEN - bitOffset) % groupSize;
        int partialBytes = (partialEntryLen / 8) + ((partialEntryLen % 8) ? 1 : 0);
        byteOffset += BYTE_IN_SIMD - partialBytes;
        bitOffset = partialBytes * 8 - partialEntryLen;
    }
}

void SimdRLEScanner::equalNormal() {
    uint8_t *uinput = (uint8_t *) input;
    uint8_t *uoutput = (uint8_t *) output;

    uint64_t entryDone = 0;

    uint64_t byteOffset = 0;
    int bitOffset = 0;
    uint8_t preserve;

    while (entryDone < this->numEntry) {
        __m512i current = _mm512_loadu_si512(uinput + byteOffset);

        __m512i eqnum = this->val1s[bitOffset];
        __m512i notmask = this->notmasks[bitOffset];

        __m512i d = _mm512_xor_si512(current, eqnum);
        __m512i result = _mm512_or_si512(
                mm512_add_epi512(_mm512_and_si512(d, notmask), notmask), d);

        if (bitOffset > rlSize) {
            preserve = uoutput[byteOffset];
            preserve &= masks[bitOffset-rlSize];
        }
        _mm512_storeu_si512((__m512i *) (uoutput + byteOffset), result);
        if (bitOffset > rlSize) {
            uoutput[byteOffset] |= preserve;
        }

        entryDone += (SIMD_LEN - bitOffset) / groupSize;

        int partialEntryLen = (SIMD_LEN - bitOffset) % groupSize;
        int partialBytes = (partialEntryLen / 8) + ((partialEntryLen % 8) ? 1 : 0);
        byteOffset += BYTE_IN_SIMD - partialBytes;
        bitOffset = partialBytes * 8 - partialEntryLen;
    }
}

void SimdRLEScanner::aless() {
    uint8_t *bytedest = (uint8_t *) output;

    int bitOffset = 0;

    uint64_t numBit = groupSize * numEntry;
    uint64_t numLane = numBit / SIMD_LEN + (numBit % SIMD_LEN ? 1 : 0);

    uint64_t laneCounter = 0;

    __m512i prev;

    while (laneCounter < numLane) {
        __m512i mask = this->msbmasks[bitOffset];
        __m512i aornm = this->nmval1s[bitOffset];
        __m512i na = this->nval1s[bitOffset];

        __m512i current = _mm512_stream_load_si512(input + laneCounter);

        __m512i xorm = _mm512_or_si512(current, mask);
        __m512i l = mm512_sub_epi512(xorm, aornm);
        __m512i result = _mm512_and_si512(_mm512_or_si512(current, na),
                                          _mm512_or_si512(_mm512_and_si512(current, na), l));

        _mm512_stream_si512(output + (laneCounter++), result);
        if (bitOffset > rlSize) {
            // Has remain to process
            int entryOffset = bitOffset - rlSize;

            int num = buildRlePiece(prev, current, entrySize, entryOffset);

            int remainIdx = (entryOffset) / 8;
            int remainOffset = (entryOffset) % 8;
            uint32_t remain = (num >= predicate->getVal1()) << (remainOffset - 1);
            uint8_t set = bytedest[(laneCounter - 1) * BYTE_IN_SIMD + remainIdx];
            set &= invmasks[remainOffset];
            set |= remain;
            bytedest[(laneCounter - 1) * BYTE_IN_SIMD + remainIdx] = set;
        }

        bitOffset = groupSize - (SIMD_LEN - bitOffset) % groupSize;

        prev = current;
    }
}

void SimdRLEScanner::lessNormal() {
    uint8_t *uinput = (uint8_t *) input;
    uint8_t *uoutput = (uint8_t *) output;

    uint64_t entryDone = 0;

    uint64_t byteOffset = 0;
    int bitOffset = 0;
    uint8_t preserve;

    while (entryDone < this->numEntry) {
        __m512i current = _mm512_loadu_si512(uinput + byteOffset);

        __m512i mask = this->msbmasks[bitOffset];
        __m512i aornm = this->nmval1s[bitOffset];
        __m512i na = this->nval1s[bitOffset];

        __m512i xorm = _mm512_or_si512(current, mask);
        __m512i l = mm512_sub_epi512(xorm, aornm);
        __m512i result = _mm512_and_si512(_mm512_or_si512(current, na),
                                          _mm512_or_si512(_mm512_and_si512(current, na), l));

        if (bitOffset > rlSize) {
            preserve = uoutput[byteOffset];
            preserve &= masks[bitOffset-rlSize];
        }
        _mm512_storeu_si512((__m512i *) (uoutput + byteOffset), result);
        if (bitOffset > rlSize) {
            uoutput[byteOffset] |= preserve;
        }

        entryDone += (SIMD_LEN - bitOffset) / groupSize;

        int partialEntryLen = (SIMD_LEN - bitOffset) % groupSize;
        int partialBytes = (partialEntryLen / 8) + ((partialEntryLen % 8) ? 1 : 0);
        byteOffset += BYTE_IN_SIMD - partialBytes;
        bitOffset = partialBytes * 8 - partialEntryLen;
    }
}

void SimdRLEScanner::lessFast() {
    uint8_t *uinput = (uint8_t *) input;
    uint8_t *uoutput = (uint8_t *) output;

    uint64_t entryDone = 0;

    uint64_t byteOffset = 0;
    int bitOffset = 0;
    uint8_t preserve;

    while (entryDone < this->numEntry) {
        __m512i current = _mm512_loadu_si512(uinput + byteOffset);

        __m512i mask = this->msbmasks[bitOffset];
        __m512i aornm = this->nmval1s[bitOffset];
        __m512i na = this->nval1s[bitOffset];

        __m512i xorm = _mm512_or_si512(current, mask);
        __m512i l = mm512_sub_epi512(xorm, aornm);
        __m512i result = _mm512_and_si512(_mm512_or_si512(current, na),
                                          _mm512_or_si512(_mm512_and_si512(current, na), l));

        _mm512_storeu_si512((__m512i *) (uoutput + byteOffset), result);

        entryDone += (SIMD_LEN - bitOffset) / groupSize;

        int partialEntryLen = (SIMD_LEN - bitOffset) % groupSize;
        int partialBytes = (partialEntryLen / 8) + ((partialEntryLen % 8) ? 1 : 0);
        byteOffset += BYTE_IN_SIMD - partialBytes;
        bitOffset = partialBytes * 8 - partialEntryLen;
    }
}