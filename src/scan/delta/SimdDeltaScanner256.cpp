//
// Created by Harper on 10/10/17.
//

#include "SimdDeltaScanner256.h"
#include <immintrin.h>

#define SIMD_LEN 256
#define SHORT_IN_SIMD 16
#define NORMAL_IN_SIMD 8

static const __m256i SHIFT16 = _mm256_set1_epi64x(16);
static const __m256i MASK16 = _mm256_set1_epi32(0xffff);
static const __m256i MASK32 = _mm256_set1_epi64x(0xffffffff);

SimdDeltaScanner256::SimdDeltaScanner256(bool shortMode) {
    this->shortMode = shortMode;
}

SimdDeltaScanner256::~SimdDeltaScanner256() {

}

void SimdDeltaScanner256::scan(int *input, uint64_t length, int *output, Predicate *p) {
    __m256i *simdin = (__m256i *) input;
    __mmask16 *maskout = (__mmask16 *) output;
    if (shortMode) {
        uint64_t numSimd = (length / SHORT_IN_SIMD) + (length % SHORT_IN_SIMD ? 1 : 0);

        __m256i a = _mm256_set1_epi16(p->getVal1());
        __m256i b = _mm256_set1_epi16(p->getVal2());

        switch (p->getOpr()) {
            case opr_eq:
            case opr_neq:
                for (uint64_t i = 0; i < numSimd; i++) {
                    __m256i current = _mm256_stream_load_si256(simdin + i);

                    __m256i aligned = _mm256_bsrli_epi128(current, 16);
                    __m256i s1 = _mm256_hadd_epi16(current, aligned);
                    __m256i s2 = _mm256_srlv_epi64(s1, SHIFT16);
                    __m256i s3 = _mm256_hadd_epi16(s1, s2);
                    __m256i s4 = _mm256_and_si256(s3, MASK16);

                    __m256i extracted = _mm256_hadd_epi16(s3, s4);

                    __mmask16 result = _mm256_cmpeq_epi16_mask(extracted, a);
                    maskout[i] = result;
                }
                break;
            case opr_in:
                for (uint64_t i = 0; i < numSimd; i++) {
                    __m256i current = _mm256_stream_load_si256(simdin + i);

                    __m256i aligned = _mm256_bsrli_epi128(current, 16);
                    __m256i s1 = _mm256_hadd_epi16(current, aligned);
                    __m256i s2 = _mm256_srlv_epi64(s1, SHIFT16);
                    __m256i s3 = _mm256_hadd_epi16(s1, s2);
                    __m256i s4 = _mm256_and_si256(s3, MASK16);

                    __m256i extracted = _mm256_hadd_epi16(s3, s4);

                    __mmask16 lower = _mm256_cmpge_epi16_mask(a, extracted);
                    __mmask16 higher = _mm256_cmple_epi16_mask(b, extracted);

                    maskout[i] = lower & higher;
                }
                break;
        }
    } else {
        uint64_t numSimd = (length / NORMAL_IN_SIMD) + (length % NORMAL_IN_SIMD ? 1 : 0);

        __m256i a = _mm256_set1_epi32(p->getVal1());
        __m256i b = _mm256_set1_epi32(p->getVal2());

        switch (p->getOpr()) {
            case opr_eq:
            case opr_neq:
                for (uint64_t i = 0; i < numSimd; i++) {
                    __m256i current = _mm256_stream_load_si256(simdin + i);

                    __m256i aligned = _mm256_bsrli_epi128(current, 32);
                    __m256i s1 = _mm256_hadd_epi32(current, aligned);
                    __m256i s2 = _mm256_and_si256(s1, MASK32);
                    __m256i extracted = _mm256_hadd_epi32(s1, s2);

                    __mmask8 result = _mm256_cmpeq_epi32_mask(extracted, a);
                    maskout[i] = result;
                }
                break;
            case opr_in:
                for (uint64_t i = 0; i < numSimd; i++) {
                    __m256i current = _mm256_stream_load_si256(simdin + i);

                    __m256i aligned = _mm256_bsrli_epi128(current, 32);
                    __m256i s1 = _mm256_hadd_epi32(current, aligned);
                    __m256i s2 = _mm256_and_si256(s1, MASK32);
                    __m256i extracted = _mm256_hadd_epi32(s1, s2);

                    __mmask8 lower = _mm256_cmpge_epi32_mask(a, extracted);
                    __mmask8 higher = _mm256_cmple_epi32_mask(b, extracted);

                    maskout[i] = lower & higher;
                }
                break;
        }
    }
}