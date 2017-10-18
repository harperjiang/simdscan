//
// Created by harper on 10/9/17.
//

#include <immintrin.h>
#include "SimdDeltaScanner128.h"

#define SIMD_LEN 128

static const __m128i SHIFT16 = _mm_setr_epi32(16, 0, 16, 0);
static const __m128i MASK16 = _mm_set1_epi32(0xffff);
static const __m128i MASK32 = _mm_setr_epi32(0xffffffff, 0, 0xffffffff, 0);

SimdDeltaScanner128::SimdDeltaScanner128(bool shortMode) {
    this->shortMode = shortMode;
}

SimdDeltaScanner128::~SimdDeltaScanner128() {

}

void SimdDeltaScanner128::scan(int *input, uint64_t length, int *output, Predicate *p) {
    __m128i *simdin = (__m128i *) input;
    __mmask8 *maskout = (__mmask8 *) output;
    if (shortMode) {
        uint64_t numSimd = (length / 8) + (length % 8 ? 1 : 0);

        __m128i a = _mm_set1_epi16(p->getVal1());
        __m128i b = _mm_set1_epi16(p->getVal2());

        int last = 0;
        switch (p->getOpr()) {
            case opr_eq:
            case opr_neq:
                for (uint64_t i = 0; i < numSimd; i++) {
                    __m128i current = _mm_stream_load_si128(simdin + i);

                    __m128i aligned = _mm_bslli_si128(current, 16);
                    __m128i s1 = _mm_hadd_epi16(current, aligned);
                    __m128i s2 = _mm_sllv_epi64(s1, SHIFT16);
                    __m128i s3 = _mm_hadd_epi16(s1, s2);
                    __m128i s4 = _mm_and_si128(s3, MASK16);

                    __m128i extracted = _mm_hadd_epi16(s3, s4);
                    extracted = _mm_add_epi16(extracted, _mm_set1_epi16(last));
                    last = _mm_extract_epi16(extracted, 0);
                    __mmask8 result = _mm_cmpeq_epi16_mask(extracted, a);
                    maskout[i] = result;
                }
                break;
            case opr_in:
                for (uint64_t i = 0; i < numSimd; i++) {
                    __m128i current = _mm_stream_load_si128(simdin + i);

                    __m128i aligned = _mm_bslli_si128(current, 16);
                    __m128i s1 = _mm_hadd_epi16(current, aligned);
                    __m128i s2 = _mm_sllv_epi64(s1, SHIFT16);
                    __m128i s3 = _mm_hadd_epi16(s1, s2);
                    __m128i s4 = _mm_and_si128(s3, MASK16);

                    __m128i extracted = _mm_hadd_epi16(s3, s4);
                    extracted = _mm_add_epi16(extracted, _mm_set1_epi16(last));
                    last = _mm_extract_epi16(extracted, 0);

                    __mmask8 lower = _mm_cmpge_epi16_mask(a, extracted);
                    __mmask8 higher = _mm_cmple_epi16_mask(b, extracted);

                    maskout[i] = lower & higher;
                }
                break;
        }
    } else {
        uint64_t numSimd = (length / 4) + (length % 4 ? 1 : 0);

        __m128i a = _mm_set1_epi32(p->getVal1());
        __m128i b = _mm_set1_epi32(p->getVal2());
        int last = 0;
        switch (p->getOpr()) {
            case opr_eq:
            case opr_neq:
                for (uint64_t i = 0; i < numSimd; i++) {
                    __m128i current = _mm_stream_load_si128(simdin + i);

                    __m128i aligned = _mm_bsrli_si128(current, 32);
                    __m128i s1 = _mm_hadd_epi32(current, aligned);
                    __m128i s2 = _mm_and_si128(s1, MASK32);
                    __m128i extracted = _mm_hadd_epi32(s1, s2);
                    extracted = _mm_add_epi32(extracted, _mm_set1_epi32(last));
                    last = _mm_extract_epi32(extracted, 0);
                    __mmask8 result = _mm_cmpeq_epi32_mask(extracted, a);
                    maskout[i] = result;
                }
                break;
            case opr_in:
                for (uint64_t i = 0; i < numSimd; i++) {
                    __m128i current = _mm_stream_load_si128(simdin + i);

                    __m128i aligned = _mm_bsrli_si128(current, 32);
                    __m128i s1 = _mm_hadd_epi32(current, aligned);
                    __m128i s2 = _mm_and_si128(s1, MASK32);
                    __m128i extracted = _mm_hadd_epi32(s1, s2);

                    extracted = _mm_add_epi32(extracted, _mm_set1_epi32(last));
                    last = _mm_extract_epi32(extracted, 0);

                    __mmask8 lower = _mm_cmp_epi32_mask(a, extracted, _MM_CMPINT_LE);
                    __mmask8 higher = _mm_cmp_epi32_mask(b, extracted, _MM_CMPINT_GE);

                    maskout[i] = lower & higher;
                }
                break;
        }
    }
}