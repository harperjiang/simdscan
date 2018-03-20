//
// Created by Harper on 10/3/17.
//
#include "BitWeaverHScanner128.h"
#include <immintrin.h>

#define SIMD_LEN 128
#define WORD_IN_SIMD 2


__m128i make128(int num, int entrySize) {
    int entryInWord = 64 / (entrySize + 1);
    uint64_t word = 0;
    for (int i = 0; i < entryInWord; i++) {
        word |= num << i * (entrySize + 1);
    }
    return _mm_setr_epi64(_mm_set_pi64x(word), _mm_set_pi64x(word));
}

__m128i makeMask128(int entrySize) {
    int singleMask = (1 << (entrySize + 1)) - 1;
    return make128(singleMask, entrySize);
}


BitWeaverHScanner128::BitWeaverHScanner128(int entrySize) {
    this->entrySize = entrySize;
}

BitWeaverHScanner128::~BitWeaverHScanner128() {

}

void BitWeaverHScanner128::scan(int *input, uint64_t numEntry, int *output, Predicate *p) {

    __m128i *simdinput = (__m128i *) input;
    __m128i *simdoutput = (__m128i *) output;
    uint64_t entryInWord = 64 / (entrySize + 1);
    uint64_t numLongWord = numEntry / entryInWord + (numEntry % entryInWord ? 1 : 0);
    uint64_t numSimd = numLongWord / WORD_IN_SIMD + (numLongWord % WORD_IN_SIMD ? 1 : 0);

    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq: {
            __m128i mbpMask = make128(1 << entrySize, entrySize);
            __m128i rbMask = make128((1 << entrySize) - 1, entrySize);
            __m128i eq = make128(p->getVal1(), entrySize);
            for (uint64_t i = 0; i < numSimd; i++) {
                __m128i current = _mm_stream_load_si128(simdinput + i);
                __m128i out = _mm_and_si128(_mm_add_epi64(_mm_xor_si128(current, eq), rbMask), mbpMask);
                _mm_stream_si128(simdoutput + i, out);
            }
            break;
        }
        case opr_in: {
            __m128i low = make128(p->getVal1(), entrySize);
            __m128i high = make128(p->getVal2(), entrySize);
            __m128i mbpMask = make128(1 << entrySize, entrySize);
            __m128i rbMask = make128((1 << entrySize) - 1, entrySize);
            for (uint64_t i = 0; i < numSimd; i++) {
                __m128i current = _mm_stream_load_si128(simdinput + i);
                __m128i cmplow = _mm_and_si128(_mm_add_epi64(_mm_xor_si128(low, rbMask), current), mbpMask);
                __m128i cmphigh = _mm_and_si128(_mm_add_epi64(_mm_xor_si128(current, rbMask), high), mbpMask);
                __m128i output = _mm_and_si128(cmplow, cmphigh);
                _mm_stream_si128(simdoutput + i, output);
            }
            break;
        }
        default:
            break;
    }

}