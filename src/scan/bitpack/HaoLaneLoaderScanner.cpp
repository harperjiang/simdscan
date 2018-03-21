//
// Created by harper on 3/20/18.
//

#include "HaoLaneLoaderScanner.h"
#include "../../util/ll/LaneLoader64.h"

#define LANE_LEN 32

static __m512i build(int num, int bitLength) {
    uint64_t val = 0;
    uint32_t current = 0;
    while (current < LANE_LEN) {
        val |= (num << current);
        current += bitLength;
    }
    return _mm512_set1_epi64(val);
}

static const __m512i one = _mm512_set1_epi32(0xffffffff);

HaoLaneLoaderScanner::HaoLaneLoaderScanner(uint32_t es) {
    this->entrySize = es;
    this->laneLoader = new LaneLoader64(es);

    this->val1 = (__m512i *) aligned_alloc(64, 64);
    this->notmask = (__m512i *) aligned_alloc(64, 64);
    this->mask = (__m512i *) aligned_alloc(64, 64);
    this->aornm = (__m512i *) aligned_alloc(64, 64);
    this->na = (__m512i *) aligned_alloc(64, 64);

    this->mask[0] = build(1 << (entrySize - 1), entrySize);
    this->notmask[0] = _mm512_xor_si512(this->mask[0], one);
}

HaoLaneLoaderScanner::~HaoLaneLoaderScanner() {
    delete this->laneLoader;
    free(this->val1);
    free(this->notmask);
    free(this->mask);
    free(this->aornm);
    free(this->na);
}

void HaoLaneLoaderScanner::scan(int *input, uint64_t length, int *output, Predicate *p) {
    uint8_t *bytein = (uint8_t *) input;

    __m512i *out = (__m512i *) output;

    uint64_t byteOffset = 0;
    uint8_t offset = 0;
    uint64_t numEntryDone = 0;
    uint64_t outputCounter = 0;

    this->val1[0] = build(p->getVal1(), entrySize);
    this->na[0] = _mm512_xor_si512(this->val1[0], one);
    this->aornm[0] = _mm512_and_si512(this->val1[0], this->notmask[0]);


    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
            uint32_t loadedEntries;
            uint32_t increasedBytes;
            uint32_t newOffset;
            while (numEntryDone < length) {
                __m512i eqnum = this->val1[0];
                __m512i nmask = this->notmask[0];
                __m512i current = laneLoader->load(bytein + byteOffset, offset, &increasedBytes, &newOffset,
                                                   &loadedEntries);
                __m512i d = _mm512_xor_si512(current, eqnum);
                __m512i result = _mm512_or_si512(
                        _mm512_add_epi64(_mm512_and_si512(d, nmask), nmask), d);

                _mm512_stream_si512(out + outputCounter++, result);
                numEntryDone += loadedEntries;
                byteOffset += increasedBytes;
                offset = newOffset;
            }
            break;
        case opr_less:
            while (numEntryDone < length) {
                __m512i mask = this->mask[0];
                __m512i aornm = this->aornm[0];
                __m512i na = this->na[0];

                __m512i current = laneLoader->load(bytein + byteOffset, offset, &increasedBytes, &newOffset,
                                                   &loadedEntries);
                __m512i xorm = _mm512_or_si512(current, mask);
                __m512i l = _mm512_sub_epi64(xorm, aornm);
                __m512i result = _mm512_and_si512(_mm512_or_si512(current, na),
                                                  _mm512_or_si512(_mm512_and_si512(current, na), l));

                _mm512_stream_si512(out + outputCounter++, result);

                numEntryDone += loadedEntries;
                byteOffset += increasedBytes;
                offset = newOffset;
            }
            break;
        default:
            break;
    }
}