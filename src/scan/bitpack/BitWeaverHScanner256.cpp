//
// Created by Harper on 10/4/17.
//

#include "BitWeaverHScanner256.h"
#include <immintrin.h>

#define SIMD_LEN 256
#define WORD_IN_SIMD 4


__m256i make256(uint64_t num, int entrySize) {
    int entryInWord = 64 / (entrySize + 1);
    uint64_t word = 0;
    for (int i = 0; i < entryInWord; i++) {
        word |= num << i * (entrySize + 1);
    }
    return _mm256_set1_epi64x(word);
}

__m256i makeMask256(int entrySize) {
    int singleMask = (1 << (entrySize + 1)) - 1;
    return make256(singleMask, entrySize);
}


BitWeaverHScanner256::BitWeaverHScanner256(int entrySize) {
    this->entrySize = entrySize;
}

BitWeaverHScanner256::~BitWeaverHScanner256() {

}

void BitWeaverHScanner256::scan(int *input, uint64_t numEntry, int *output, Predicate *p) {

    __m256i *simdinput = (__m256i *) input;
    __m256i *simdoutput = (__m256i *) output;
    uint64_t entryInWord = 64 / (entrySize + 1);
    uint64_t numLongWord = numEntry / entryInWord + (numEntry % entryInWord ? 1 : 0);
    uint64_t numSimd = numLongWord / WORD_IN_SIMD + (numLongWord % WORD_IN_SIMD ? 1 : 0);

    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq: {
            __m256i eq = make256(p->getVal1(), entrySize);
            __m256i mbpMask = make256(1 << entrySize, entrySize);
            __m256i rbMask = make256((1 << entrySize) - 1, entrySize);
            for (uint64_t i = 0; i < numSimd; i++) {
                __m256i current = _mm256_stream_load_si256(simdinput + i);
                __m256i out = _mm256_and_si256(_mm256_add_epi64(_mm256_xor_si256(current, eq), rbMask), mbpMask);
                _mm256_stream_si256(simdoutput + i, out);
            }
            break;
        }
        case opr_in: {
            __m256i low = make256(p->getVal1(), entrySize);
            __m256i high = make256(p->getVal2(), entrySize);
            __m256i mbpMask = make256(1 << entrySize, entrySize);
            __m256i rpMask = make256((1 << entrySize) - 1, entrySize);
            for (uint64_t i = 0; i < numSimd; i++) {
                __m256i current = _mm256_stream_load_si256(simdinput + i);
                __m256i cmplow = _mm256_and_si256(_mm256_add_epi64(_mm256_xor_si256(low, rpMask), current), mbpMask);
                __m256i cmphigh = _mm256_and_si256(_mm256_add_epi64(_mm256_xor_si256(current, rpMask), high), mbpMask);
                __m256i output = _mm256_and_si256(cmplow, cmphigh);
                _mm256_stream_si256(simdoutput + i, output);
            }
            break;
        }
        default:
            break;
    }

}