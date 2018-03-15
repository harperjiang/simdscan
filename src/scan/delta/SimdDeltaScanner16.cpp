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
    // Use 512 bit SIMD as buffer, so do not need to consider overflow
    this->shuffleInst = new __m512i[8];
    this->shiftInst = new __m512i[8];
    this->unpackMask = new __m256i[8];

    uint32_t shubuffer[16];
    uint32_t sftbuffer[16];

    for (int offset = 0; offset < 8; offset++) {
        // 16 entries
        for (int j = 0; j < 16; j++) {
            shubuffer[j] = 0;
            uint32_t bitLoc = offset + j * entrySize;
            uint32_t byteLoc = bitLoc / 8;
            int bitsDone = 0;
            // 1st-th byte
            shubuffer[j] |= byteLoc;
            bitsDone += 8 - bitLoc % 8;
            int remain = (entrySize - bitsDone) / 8 + ((entrySize - bitsDone) % 8 ? 1 : 0);
            for (int k = 1; k <= remain; k++) {
                shubuffer[j] |= (byteLoc + k) << k * 8;
            }
            sftbuffer[j] = bitLoc % 8; // Erase previous
        }


        shuffleInst[offset] = _mm512_setr_epi32(shubuffer[0], shubuffer[1], shubuffer[2], shubuffer[3],
                                                shubuffer[4], shubuffer[5], shubuffer[6], shubuffer[7],
                                                shubuffer[8], shubuffer[9], shubuffer[10], shubuffer[11],
                                                shubuffer[12], shubuffer[13], shubuffer[14], shubuffer[15]);
        shiftInst[offset] = _mm512_setr_epi32(sftbuffer[0], sftbuffer[1], sftbuffer[2], sftbuffer[3],
                                              sftbuffer[4], sftbuffer[5], sftbuffer[6], sftbuffer[7],
                                              sftbuffer[8], sftbuffer[9], sftbuffer[10], sftbuffer[11],
                                              sftbuffer[12], sftbuffer[13], sftbuffer[14], sftbuffer[15]);
        unpackMask[offset] = _mm256_set1_epi16((1 << entrySize) - 1);
    }

}

SimdDeltaScanner16::~SimdDeltaScanner16() {
    delete[] this->shuffleInst;
    delete[] this->shiftInst;
}

__m256i SimdDeltaScanner16::unpack(__m256i &input, int offset) {
    __m512i data = _mm512_castsi256_si512(input);
    data = _mm512_permutexvar_epi8(data, shuffleInst[offset]);
    data = _mm512_sllv_epi16(data, shiftInst[offset]);
    return _mm256_and_si256(_mm512_cvtepi32_epi16(data), unpackMask[offset]);
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
                __m256i current = _mm256_loadu_si256((__m256i *) (bytein + byteCounter));
                current = unpack(current, offset);
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

                maskout[outputCounter++] = _mm256_cmpeq_epi16_mask(extracted, a);

                entryCounter += 16;

                int bitAdvance = offset + entrySize * 16;
                byteCounter += bitAdvance / 8;
                offset = bitAdvance % 8;
            }
            break;
        case opr_less:
            while (entryCounter < length) {
                __m256i current = _mm256_loadu_si256((__m256i *) (bytein + byteCounter));
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

                maskout[outputCounter++] = _mm256_cmpge_epi16_mask(a, extracted);
                entryCounter += 16;

                int bitAdvance = offset + entrySize * 16;
                byteCounter += bitAdvance / 8;
                offset = bitAdvance % 8;
            }
            break;
    }

}