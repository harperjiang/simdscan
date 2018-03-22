//
// Created by harper on 3/14/18.
//

#include "LemireDeltaScanner.h"
#include "../../util/unpack/u256/Small32Unpacker.h"
#include "../../util/unpack/u256/Large32Unpacker.h"

LemireDeltaScanner::LemireDeltaScanner(uint32_t es) {
    this->entrySize = es;
    if (es < 26)
        this->unpacker = new Small32Unpacker(es);
    else
        this->unpacker = new Large32Unpacker(es);
}

LemireDeltaScanner::~LemireDeltaScanner() {
    delete this->unpacker;
}

void LemireDeltaScanner::scan(int *input, uint64_t length, int *output, Predicate *p) {
    uint8_t *bytein = (uint8_t *) input;

    __m256i a = _mm256_set1_epi32(p->getVal1());

    __mmask8 *maskout = (__mmask8 *) output;


    uint64_t byteOffset = 0;
    uint8_t offset = 0;
    uint64_t numEntryDone = 0;
    uint64_t outputCounter = 0;

    __m256i current = _mm256_set1_epi32(0);
    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
            while (numEntryDone < length) {
                __m256i unpacked = unpacker->unpack(bytein + byteOffset, offset);
                __m256i cumsum = _mm256_add_epi32(current, unpacked);
                __mmask8 result = _mm256_cmpeq_epi32_mask(cumsum, a);
                maskout[outputCounter++] = result;

                numEntryDone += 8;
                uint64_t bitAdvance = offset + 8 * entrySize;
                byteOffset += bitAdvance / 8;
                offset = bitAdvance % 8;
            }
            break;
        case opr_less:
            while (numEntryDone < length) {
                __m256i unpacked = unpacker->unpack(bytein + byteOffset, offset);
                __m256i cumsum = _mm256_add_epi32(current, unpacked);
                __mmask8 result = _mm256_cmpeq_epi32_mask(cumsum, a);
                maskout[outputCounter++] = result;

                numEntryDone += 8;
                uint64_t bitAdvance = offset + 8 * entrySize;
                byteOffset += bitAdvance / 8;
                offset = bitAdvance % 8;
            }
            break;
    }
}