//
// Created by harper on 3/20/18.
//

#include "HaoLaneLoaderScanner.h"
#include "../../util/ll/LaneLoader64.h"

HaoLaneLoaderScanner::HaoLaneLoaderScanner(uint32_t es) {
    this->entrySize = es;
    this->laneLoader = new LaneLoader64(es);
}

HaoLaneLoaderScanner::~HaoLaneLoaderScanner() {
    delete this->laneLoader;
}

void HaoLaneLoaderScanner::scan(int *input, uint64_t length, int *output, Predicate *p) {
    uint8_t *bytein = (uint8_t *) input;

    __m512i *predout = (__m512i *) output;

    uint64_t byteOffset = 0;
    uint8_t offset = 0;
    uint64_t numEntryDone = 0;
    uint64_t outputCounter = 0;

    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
//            while (numEntryDone < length) {
//                __m512i current = laneLoader->load(bytein + byteOffset, offset);
//
//                __mmask8 result = _mm256_cmpeq_epi32_mask(current, a32);
//                mask32out[outputCounter++] = result;
//
//                numEntryDone += 8;
//                uint64_t bitAdvance = offset + 8 * entrySize;
//                byteOffset += bitAdvance / 8;
//                offset = bitAdvance % 8;
//            }
            break;
        case opr_less:
//            while (numEntryDone < length) {
//                __m256i current = unpacker->unpack(bytein + byteOffset, offset);
//
//                __mmask8 lower = _mm256_cmp_epi32_mask(current, a32, _MM_CMPINT_LT);
//
//                mask32out[outputCounter++] = lower;
//
//                numEntryDone += 8;
//                uint64_t bitAdvance = offset + 8 * entrySize;
//                byteOffset += bitAdvance / 8;
//                offset = bitAdvance % 8;
//            }
            break;
    }
}