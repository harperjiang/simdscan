//
// Created by Harper on 10/10/17.
//

#include "SimdDeltaScanner32.h"
#include "../../util/unpack/Large32Unpacker.h"
#include "../../util/unpack/Small32Unpacker.h"
#include "../../util/unpack/TrivialUnpacker.h"
#include <immintrin.h>
#include <assert.h>

#define SIMD_LEN 256
#define SHORT_IN_SIMD 16
#define NORMAL_IN_SIMD 8

static const __m256i SHIFT16 = _mm256_set1_epi64x(16);
static const __m256i MASK16 = _mm256_set1_epi32(0xffff);

static const __m256i ZERO = _mm256_set1_epi32(0);
static const __m256i IDX = _mm256_setr_epi32(8, 0, 1, 2, 3, 4, 5, 6);
static const __m256i IDX2 = _mm256_setr_epi32(0, 8, 2, 8, 1, 4, 3, 6);
static const __m256i IDX3 = _mm256_setr_epi32(8, 8, 8, 8, 0, 1, 2, 3);
static const __m256i MASK32 = _mm256_set1_epi64x(0xffffffff00000000);

SimdDeltaScanner32::SimdDeltaScanner32(int es) {
    this->entrySize = es;
    int entryInSimd = SIMD_LEN / es;
    assert(es > 16 && es <= 32);
    // 32 bit mode, 8 entries per word

    if (es > 25 && es < 32) {
        unpacker = new Large32Unpacker(es);
    } else if (es == 32) {
        unpacker = new TrivialUnpacker();
    } else {
        unpacker = new Small32Unpacker(es);
    }
}

SimdDeltaScanner32::~SimdDeltaScanner32() {
    delete this->unpacker;
}

void SimdDeltaScanner32::scan(int *input, uint64_t length, int *output, Predicate *p) {
    uint8_t *bytein = (uint8_t *) input;

    __m256i a = _mm256_set1_epi32(p->getVal1());

    __mmask8 *maskout = (__mmask8 *) output;


    uint64_t byteOffset = 0;
    uint8_t offset = 0;
    uint64_t numEntryDone = 0;
    uint64_t outputCounter = 0;

    int cumsum = 0;
    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
            while (numEntryDone < length) {
                __m256i current = unpacker->unpack(bytein + byteOffset, offset);

                __m256i aligned = _mm256_permutex2var_epi32(current, IDX, ZERO);
                __m256i s1 = _mm256_hadd_epi32(current, aligned);
                __m256i s2 = _mm256_permutex2var_epi32(s1, IDX2, ZERO);
                __m256i s3 = _mm256_hadd_epi32(s1, s2);
                __m256i s4 = _mm256_permutex2var_epi32(s3, IDX3, ZERO);
                __m256i extracted = _mm256_add_epi32(s3, s4);
                __m256i start = _mm256_set1_epi32(cumsum);
                extracted = _mm256_add_epi32(start, extracted);
                cumsum = _mm256_extract_epi32(extracted, 4);

                __mmask8 result = _mm256_cmpeq_epi32_mask(extracted, a);
                maskout[outputCounter++] = result;

                numEntryDone += 8;
                uint64_t bitAdvance = offset + 8 * entrySize;
                byteOffset += bitAdvance / 8;
                offset = bitAdvance % 8;
            }
            break;
        case opr_less:
            while (numEntryDone < length) {
                __m256i current = unpacker->unpack(bytein + byteOffset, offset);

                __m256i aligned = _mm256_permutex2var_epi32(current, IDX, ZERO);
                __m256i s1 = _mm256_hadd_epi32(current, aligned);
                __m256i s2 = _mm256_permutex2var_epi32(s1, IDX2, ZERO);
                __m256i s3 = _mm256_hadd_epi32(s1, s2);
                __m256i s4 = _mm256_permutex2var_epi32(s3, IDX3, ZERO);
                __m256i extracted = _mm256_add_epi32(s3, s4);
                __m256i start = _mm256_set1_epi32(cumsum);
                extracted = _mm256_add_epi32(start, extracted);
                cumsum = _mm256_extract_epi32(extracted, 4);

                __mmask8 lower = _mm256_cmp_epi32_mask(extracted, a, _MM_CMPINT_LT);

                maskout[outputCounter++] = lower;

                numEntryDone += 8;
                uint64_t bitAdvance = offset + 8 * entrySize;
                byteOffset += bitAdvance / 8;
                offset = bitAdvance % 8;
            }
            break;
    }

}