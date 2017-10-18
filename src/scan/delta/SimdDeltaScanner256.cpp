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

static const __m256i ZERO = _mm256_set1_epi32(0);
static const __m256i IDX = _mm256_setr_epi32(8, 0, 1, 2, 3, 4, 5, 6);
static const __m256i MASK32 = _mm256_set1_epi64x(0xffffffff00000000);

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

        short cumsum = 0;
        switch (p->getOpr()) {
            case opr_eq:
            case opr_neq:
                for (uint64_t i = 0; i < numSimd; i++) {
                    __m256i current = _mm256_stream_load_si256(simdin + i);
                    __m256i aligned = _mm256_bslli_epi128(current, 16);
                    __m256i s1 = _mm256_hadd_epi16(current, aligned);
                    __m256i s2 = _mm256_sllv_epi64(s1, SHIFT16);
                    __m256i s3 = _mm256_hadd_epi16(s1, s2);
                    __m256i s4 = _mm256_and_si256(s3, MASK16);
                    __m256i extracted = _mm256_hadd_epi16(s3, s4);

                    int cs1 = _mm256_extract_epi16(extracted, 0);
                    int cs2 = _mm256_extract_epi16(extracted, 8);

                    int cus = (int) cumsum;

                    int first = cus << 16 + cus;
                    int second = (cus + cs1) << 16 + cus + cs1;
                    __m256i cumadd = _mm256_setr_epi32(first, first, first, first, second, second, second, second);
                    extracted = _mm256_add_epi32(cumadd, extracted);

                    cumsum += cs1 + cs2;

                    __mmask16 result = _mm256_cmpeq_epi16_mask(extracted, a);
                    maskout[i] = result;
                }
                break;
            case opr_in:
                for (uint64_t i = 0; i < numSimd; i++) {
                    __m256i current = _mm256_stream_load_si256(simdin + i);
                    __m256i aligned = _mm256_bslli_epi128(current, 16);
                    __m256i s1 = _mm256_hadd_epi16(current, aligned);
                    __m256i s2 = _mm256_sllv_epi64(s1, SHIFT16);
                    __m256i s3 = _mm256_hadd_epi16(s1, s2);
                    __m256i s4 = _mm256_and_si256(s3, MASK16);
                    __m256i extracted = _mm256_hadd_epi16(s3, s4);

                    int cs1 = _mm256_extract_epi16(extracted, 0);
                    int cs2 = _mm256_extract_epi16(extracted, 8);

                    int cus = (int) cumsum;

                    int first = cus << 16 + cus;
                    int second = (cus + cs1) << 16 + cus + cs1;
                    __m256i cumadd = _mm256_setr_epi32(first, first, first, first, second, second, second, second);
                    extracted = _mm256_add_epi32(cumadd, extracted);

                    cumsum += cs1 + cs2;

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
            case opr_in:
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

                    __mmask8 lower = _mm256_cmp_epi32_mask(a, extracted, _MM_CMPINT_LE);
                    __mmask8 higher = _mm256_cmp_epi32_mask(b, extracted, _MM_CMPINT_GE);

                    maskout[i] = lower & higher;
                }
                break;
        }
    }
}