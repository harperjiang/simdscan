//
// Created by Harper on 10/10/17.
//

#include "SimdDeltaScanner32.h"
#include <immintrin.h>
#include <assert.h>

#define SIMD_LEN 256
#define SHORT_IN_SIMD 16
#define NORMAL_IN_SIMD 8

static const __m256i SHIFT16 = _mm256_set1_epi64x(16);
static const __m256i MASK16 = _mm256_set1_epi32(0xffff);

static const __m256i ZERO = _mm256_set1_epi32(0);
static const __m256i IDX = _mm256_setr_epi32(8, 0, 1, 2, 3, 4, 5, 6);
static const __m256i MASK32 = _mm256_set1_epi64x(0xffffffff00000000);

SimdDeltaScanner32::SimdDeltaScanner32(int es) {
    this->entrySize = es;
    int entryInSimd = SIMD_LEN / es;
    assert(es > 16 && es < 32);
    // 32 bit mode, 8 entries per word
    this->bufferSize = entryInSimd / 8 + (entryInSimd % 8 ? 1 : 0);
    // Use 512 bit SIMD as buffer, so do not need to consider overflow
    buffer = new __m512i[this->bufferSize];
}

SimdDeltaScanner32::~SimdDeltaScanner32() {
    delete[] this->buffer;
}

void SimdDeltaScanner32::unpack(__m256i &input, int offset) {
    for (int i = 0; i < bufferSize; i++) {
        __m512i data = _mm512_castsi256_si512(input);
        data = _mm512_shuffle_epi8(data, SHUFFLE_INST[offset][i]);
        data = _mm512_sllv_epi32(data, SHIFT_INST[offfset][i]);
        buffer[i] = data;
    }
}

void SimdDeltaScanner32::scan(int *input, uint64_t length, int *output, Predicate *p) {
    __m256i *simdin = (__m256i *) input;
    uint64_t numSimd = (length / NORMAL_IN_SIMD) + (length % NORMAL_IN_SIMD ? 1 : 0);

    __m256i a = _mm256_set1_epi32(p->getVal1());
    __m256i b = _mm256_set1_epi32(p->getVal2());

    __mmask8 *maskout = (__mmask8 *) output;

    int cumsum = 0;
    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
            for (uint64_t i = 0; i < numSimd; i++) {
                __m256i current = _mm256_stream_load_si256(simdin + i);

                __m256i aligned = _mm256_permutex2var_epi32(current, ZERO, IDX);
                __m256i s1 = _mm256_hadd_epi32(current, aligned);
                __m256i s2 = _mm256_bslli_epi128(s1, 32);
                __m256i s3 = _mm256_hadd_epi32(s1, s2);
                __m256i s4 = _mm256_and_si256(s3, MASK32);
                __m256i extracted = _mm256_hadd_epi32(s3, s4);
                __m256i start = _mm256_set1_epi32(cumsum);
                extracted = _mm256_add_epi32(start, extracted);
                cumsum = _mm256_extract_epi32(extracted, 0);

                __mmask8 result = _mm256_cmpeq_epi32_mask(extracted, a);
                maskout[i] = result;
            }
            break;
        case opr_less:
            for (uint64_t i = 0; i < numSimd; i++) {
                __m256i current = _mm256_stream_load_si256(simdin + i);

                __m256i aligned = _mm256_permutex2var_epi32(current, ZERO, IDX);
                __m256i s1 = _mm256_hadd_epi32(current, aligned);
                __m256i s2 = _mm256_bslli_epi128(s1, 32);
                __m256i s3 = _mm256_hadd_epi32(s1, s2);
                __m256i s4 = _mm256_and_si256(s3, MASK32);
                __m256i extracted = _mm256_hadd_epi32(s3, s4);
                __m256i start = _mm256_set1_epi32(cumsum);
                extracted = _mm256_add_epi32(start, extracted);
                cumsum = _mm256_extract_epi32(extracted, 0);

                __mmask8 lower = _mm256_cmp_epi32_mask(extracted, a, _MM_CMPINT_LE);

                maskout[i] = lower;
            }
            break;
    }
}