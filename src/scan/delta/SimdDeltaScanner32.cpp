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

    // Use 512 bit SIMD as buffer, so do not need to consider overflow
    this->shuffleInst = new __m512i[8];
    this->shiftInst = new __m512i[8];

    uint64_t shubuffer[8];
    uint64_t sftbuffer[8];

    for (int offset = 0; offset < 8; offset++) {
        // 8 entries
        for (int j = 0; j < 8; j++) {
            shubuffer[j] = 0;
            uint64_t bitLoc = offset + j * entrySize;
            uint64_t byteLoc = bitLoc / 8;
            int bitsDone = 0;
            // 1st-th byte
            shubuffer[j] |= byteLoc;
            bitsDone += 8 - bitLoc % 8;
            int remain = (entrySize - bitsDone) / 8 + ((entrySize - bitsDone) % 8 ? 1 : 0);
            for (uint64_t k = 1; k <= remain; k++) {
                shubuffer[j] |= (byteLoc + k) << k * 8;
            }
            sftbuffer[j] = bitLoc % 8; // Erase previous
        }

        shuffleInst[offset] = _mm512_setr_epi64(shubuffer[0], shubuffer[1], shubuffer[2], shubuffer[3],
                                                shubuffer[4], shubuffer[5], shubuffer[6], shubuffer[7]);

        shiftInst[offset] = _mm512_setr_epi64(sftbuffer[0], sftbuffer[1], sftbuffer[2], sftbuffer[3],
                                              sftbuffer[4], sftbuffer[5], sftbuffer[6], sftbuffer[7]);
    }
}

SimdDeltaScanner32::~SimdDeltaScanner32() {
    delete[] shuffleInst;
    delete[] shiftInst;
}

__m256i SimdDeltaScanner32::unpack(__m256i &input, int offset) {
    __m512i data = _mm512_castsi256_si512(input);
    data = _mm512_shuffle_epi8(data, shuffleInst[offset]);
    data = _mm512_sllv_epi32(data, shiftInst[offset]);
    return _mm512_cvtepi64_epi32(data);
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
                __m256i current = _mm256_loadu_si256((__m256i *) (bytein + byteOffset));
                current = unpack(current, offset);

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
                maskout[outputCounter++] = result;

                numEntryDone += 8;
                uint64_t bitAdvance = offset + 8 * entrySize;
                byteOffset += bitAdvance / 8;
                offset = bitAdvance % 8;
            }
            break;
        case opr_less:
            while (numEntryDone < length) {
                __m256i current = _mm256_loadu_si256((__m256i *) (bytein + byteOffset));
                current = unpack(current, offset);

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

                maskout[outputCounter++] = lower;

                numEntryDone += 8;
                uint64_t bitAdvance = offset + 8 * entrySize;
                byteOffset += bitAdvance / 8;
                offset = bitAdvance % 8;
            }
            break;
    }

}