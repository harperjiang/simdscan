//
// Created by Harper on 10/10/17.
//

#include "SimdDeltaScanner16.h"
#include "../../util/unpack/u256/Small16Unpacker.h"
#include "../../util/unpack/u256/Large16Unpacker.h"
#include "../../util/unpack/u256/TrivialUnpacker.h"
#include <assert.h>

#define SIMD_LEN 256
#define SHORT_IN_SIMD 16
#define NORMAL_IN_SIMD 8

static const __m256i SHIFT16 = _mm256_set1_epi64x(16);
static const __m256i MASK16 = _mm256_set1_epi32(0xffff);
static const __m256i INV16 = _mm256_setr_epi16(0xF0E, 0xD0C, 0xB0A,
                                                         0x908, 0x706, 0x504, 0x302, 0x100, 0xF0E, 0xD0C, 0xB0A, 0x908,
                                                         0x706, 0x504, 0x302, 0x100);

static const __m256i ZERO = _mm256_set1_epi32(0);
static const __m256i IDX = _mm256_setr_epi32(8, 0, 1, 2, 3, 4, 5, 6);
static const __m256i MASK32 = _mm256_set1_epi64x(0xffffffff00000000);

SimdDeltaScanner16::SimdDeltaScanner16(int es) {
    this->entrySize = es;
    int entryInSimd = SIMD_LEN / es;
    assert(es <= 16);
    if (es <= 8) {
        this->unpacker = new Small16Unpacker(es);
    } else if (es == 16) {
        this->unpacker = new TrivialUnpacker();
    } else {
        this->unpacker = new Large16Unpacker(es);
    }
}

SimdDeltaScanner16::~SimdDeltaScanner16() {
    delete this->unpacker;
}

void SimdDeltaScanner16::scan(int *input, uint64_t length, int *output, Predicate *p) {
    uint8_t *bytein = (uint8_t *) input;
    __mmask16 *maskout = (__mmask16 *) output;

    uint64_t entryCounter = 0;
    int outputCounter = 0;

    uint64_t byteCounter = 0;
    int offset = 0;

    __m256i a = _mm256_set1_epi16(p->getVal1());

    short cumsum = 0;
    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
            while (entryCounter < length) {
                __m256i current = unpacker->unpack(bytein + byteCounter, offset);
                __m256i aligned = _mm256_bslli_epi128(current, 2);
                __m256i s1 = _mm256_hadd_epi16(current, aligned);
                __m256i s2 = _mm256_sllv_epi64(s1, SHIFT16);
                __m256i s3 = _mm256_hadd_epi16(s1, s2);
                __m256i s4 = _mm256_and_si256(s3, MASK16);
                __m256i extracted = _mm256_hadd_epi16(s3, s4);
                __m256i sorted = _mm256_shuffle_epi8(extracted, INV16);

                int cs = _mm256_extract_epi16(extracted, 15);
                int cus = (int) cumsum;

                __m256i cumadd = _mm256_set1_epi32(cus);
                sorted = _mm256_add_epi32(cumadd, sorted);

                cumsum += cs;

                maskout[outputCounter++] = _mm256_cmpeq_epi16_mask(sorted, a);

                entryCounter += 16;

                int bitAdvance = offset + entrySize * 16;
                byteCounter += bitAdvance / 8;
                offset = bitAdvance % 8;
            }
            break;
        case opr_less:
            while (entryCounter < length) {
                __m256i current = unpacker->unpack(bytein + byteCounter, offset);
                __m256i aligned = _mm256_bslli_epi128(current, 2);
                __m256i s1 = _mm256_hadd_epi16(current, aligned);
                __m256i s2 = _mm256_sllv_epi64(s1, SHIFT16);
                __m256i s3 = _mm256_hadd_epi16(s1, s2);
                __m256i s4 = _mm256_and_si256(s3, MASK16);
                __m256i extracted = _mm256_hadd_epi16(s3, s4);

                __m256i sorted = _mm256_shuffle_epi8(extracted, INV16);

                int cs = _mm256_extract_epi16(extracted, 15);
                int cus = (int) cumsum;

                __m256i cumadd = _mm256_set1_epi32(cus);
                sorted = _mm256_add_epi32(cumadd, sorted);

                cumsum += cs;

                maskout[outputCounter++] = _mm256_cmplt_epi16_mask(sorted, a);
                entryCounter += 16;

                int bitAdvance = offset + entrySize * 16;
                byteCounter += bitAdvance / 8;
                offset = bitAdvance % 8;
            }
            break;
    }

}

__m256i SimdDeltaScanner16::cumsum(__m256i current) {
    __m256i aligned = _mm256_bslli_epi128(current, 2);
    __m256i s1 = _mm256_hadd_epi16(current, aligned);
    __m256i s2 = _mm256_sllv_epi64(s1, SHIFT16);
    __m256i s3 = _mm256_hadd_epi16(s1, s2);
    __m256i s4 = _mm256_and_si256(s3, MASK16);
    __m256i extracted = _mm256_hadd_epi16(s3, s4);
    return _mm256_shuffle_epi8(extracted, INV16);
}