//
// Created by Harper on 10/10/17.
//

#include "SimdDeltaScanner16.h"
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

SimdDeltaScanner16::SimdDeltaScanner16(int es) {
    this->entrySize = es;
    int entryInSimd = SIMD_LEN / es;
    assert(es <= 16);
    // 16 bit mode, 16 entries per word
    this->bufferSize = entryInSimd / 16 + (entryInSimd % 16 ? 1 : 0);
    // Use 512 bit SIMD as buffer, so do not need to consider overflow
    buffer = new __m512i[this->bufferSize];

    this->shuffleInst =
}

SimdDeltaScanner16::~SimdDeltaScanner16() {
    delete[] this->buffer;
}

void SimdDeltaScanner16::unpack(__m256i &input, int offset) {
    for (int i = 0; i < bufferSize; i++) {
        __m512i data = _mm512_castsi256_si512(input);
        data = _mm512_shuffle_epi8(data, shuffleInst[entrySize,offset,i]);
        data = _mm512_sllv_epi16(data, shiftInst[entrySize][offset][i]);
        buffer[i] = data;
    }
}

void SimdDeltaScanner16::scan(int *input, uint64_t length, int *output, Predicate *p) {
    __m256i *simdin = (__m256i *) input;
    __mmask16 *maskout = (__mmask16 *) output;
    uint64_t numSimd = (length / SHORT_IN_SIMD) + (length % SHORT_IN_SIMD ? 1 : 0);

    __m256i a = _mm256_set1_epi16(p->getVal1());

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

                maskout[i] = _mm256_cmpeq_epi16_mask(extracted, a);
            }
            break;
        case opr_less:
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

                maskout[i] = _mm256_cmpge_epi16_mask(a, extracted);
            }
            break;
    }

}