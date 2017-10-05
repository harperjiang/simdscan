//
// Created by Harper on 10/4/17.
//

#include "BitWeaverHScanner512.h"
#include <immintrin.h>

#define SIMD_LEN 512
#define WORD_IN_SIMD SIMD_LEN / 64


__m512i make512(int num, int entrySize) {
    int entryInWord = 64 / (entrySize + 1);
    long word[8];
    for (int i = 0; i < entryInWord; i++) {
        for (int j = 0; j < 4; j++)
            word[j] |= num << i * (entrySize + 1);
    }
    return _mm512_setr_epi64(word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);
}

__m512i makeMask512(int entrySize) {
    int singleMask = (1 << (entrySize + 1)) - 1;
    return make512(singleMask, entrySize);
}


BitWeaverHScanner512::BitWeaverHScanner512(int entrySize) {
    this->entrySize = entrySize;
}

BitWeaverHScanner512::~BitWeaverHScanner512() {

}

void BitWeaverHScanner512::scan(int *input, uint64_t numEntry, int *output, Predicate *p) {

    __m512i *simdinput = (__m512i *) input;
    __m512i *simdoutput = (__m512i *) output;
    uint64_t entryInWord = 64 / (entrySize + 1);
    uint64_t numLongWord = numEntry / entryInWord + (numEntry % entryInWord ? 1 : 0);
    uint64_t numSimd = numLongWord / WORD_IN_SIMD + (numLongWord % WORD_IN_SIMD ? 1 : 0);

    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq: {
            __m512i mask = makeMask512(entrySize);
            __m512i eq = make512(p->getVal1(), entrySize);
            for (int i = 0; i < numSimd; i++) {
                __m512i current = _mm512_stream_load_si512(simdinput + i);
                __m512i out = _mm512_add_epi64(_mm512_xor_si512(current, eq), mask);
                _mm512_stream_si512(simdoutput + i, out);
            }
            break;
        }
        case opr_in: {
            __m512i low = make512(p->getVal1(), entrySize);
            __m512i high = make512(p->getVal2(), entrySize);
            __m512i mbpMask = make512(1 << entrySize, entrySize);
            __m512i rpMask = make512((1 << entrySize) - 1, entrySize);
            for (int i = 0; i < numSimd; i++) {
                __m512i current = _mm512_stream_load_si512(simdinput + i);
                __m512i cmplow = _mm512_and_si512(_mm512_add_epi64(_mm512_xor_si512(low, rpMask), current), mbpMask);
                __m512i cmphigh = _mm512_and_si512(_mm512_add_epi64(_mm512_xor_si512(current, rpMask), high), mbpMask);
                __m512i output = _mm512_and_si512(cmplow, cmphigh);
                _mm512_stream_si512(simdoutput + i, output);
            }
            break;
        }
        default:
            break;
    }

}