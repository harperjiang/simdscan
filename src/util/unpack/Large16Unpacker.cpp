//
// Created by harper on 3/15/18.
//

#include <assert.h>
#include "Large16Unpacker.h"

Large16Unpacker::Large16Unpacker(uint32_t es) {
    this->entrySize = es;
    assert(es > 8 && es < 16);

    this->nextPos = new uint8_t[24];

    this->mask = _mm256_set1_epi16((1 << es) - 1);
    this->shuffleInst = (__m512i *) aligned_alloc(64, 64 * 8);
    this->shiftInst = (__m512i *) aligned_alloc(64, 64 * 8);

    __m128i *shuffleBuffer = (__m128i *) aligned_alloc(16, 16 * 8);
    __m128i *shiftBuffer = (__m128i *) aligned_alloc(16, 16 * 8);
    uint16_t *shuffleDataBuffer = (uint16_t *) aligned_alloc(16, 16);
    uint16_t *shiftDataBuffer = (uint16_t *) aligned_alloc(16, 16);
    // Compute shuffle and shift instructions
    for (int offset = 0; offset < 8; offset++) {
        for (int idx = 0; idx < 8; idx++) {
            uint32_t entryoff = offset + entrySize * idx;
            shuffleDataBuffer[idx] = entryoff / 8;
            shiftDataBuffer[idx] = entryoff % 8;
            if (entryoff % 8 + entrySize > 8) {
                shuffleDataBuffer[idx] |= (entryoff % 8 + 1) << 8;
            } else {
                shuffleDataBuffer[idx] |= 0xff << 8;
            }
        }
        for (int j = 0; j < 3; j++) {
            nextPos[offset * 3 + j] = (offset + entrySize * 4 * (j + 1)) / 8;
        }

        shuffleBuffer[offset] = _mm_load_si128((__m128i *) shuffleDataBuffer);
        shiftBuffer[offset] = _mm_load_si128((__m128i *) shiftDataBuffer);
    }

    // Combine them to make 512-bit shuffle and shift instructions
    for (int i = 0; i < 8; i++) {
        int high = (i + entrySize * 8) % 8;
        int higher = (high + entrySize * 8) % 8;
        int evenHigher = (higher + entrySize * 8) % 8;

        __m128i su0 = shuffleBuffer[i];
        __m128i su1 = shuffleBuffer[high];
        __m128i su2 = shuffleBuffer[higher];
        __m128i su3 = shuffleBuffer[evenHigher];

        __m512i shuffle = _mm512_castsi128_si512(su0);
        shuffle = _mm512_inserti64x2(shuffle, su1, 1);
        shuffle = _mm512_inserti64x2(shuffle, su2, 2);
        shuffle = _mm512_inserti64x2(shuffle, su3, 3);
        this->shuffleInst[i] = shuffle;

        __m128i sh0 = shuffleBuffer[i];
        __m128i sh1 = shuffleBuffer[high];
        __m128i sh2 = shuffleBuffer[higher];
        __m128i sh3 = shuffleBuffer[evenHigher];

        __m512i shift = _mm512_castsi128_si512(sh0);
        shift = _mm512_inserti64x2(shift, sh1, 1);
        shift = _mm512_inserti64x2(shift, sh2, 2);
        shift = _mm512_inserti64x2(shift, sh3, 3);
        this->shiftInst[i] = shift;
    }

    free(shuffleDataBuffer);
    free(shiftDataBuffer);
    free(shuffleBuffer);
    free(shiftBuffer);
}

Large16Unpacker::~Large16Unpacker() {
    free(this->shuffleInst);
    free(this->shiftInst);
    delete[] this->nextPos;
}

__m256i Large16Unpacker::unpack(uint8_t *data, uint8_t offset) {
    // Load 4 128 bits, each contains 4 entries to build a 512-bit SIMD
    __m256i lower = _mm256_loadu2_m128i((__m128i *) (data + nextPos[offset]), (__m128i *) data);
    __m256i higher = _mm256_loadu2_m128i((__m128i *) (data + nextPos[offset + 2]),
                                         (__m128i *) (data + nextPos[offset + 1]));
    // Get a single 512 bit
    __m512i main = _mm512_castsi256_si512(lower);
    main = _mm512_inserti64x4(main, higher, 1);

    // Shuffle bytes
    main = _mm512_shuffle_epi8(main, shuffleInst[offset]);
    // Shift bits
    main = _mm512_sllv_epi16(main, shiftInst[offset]);
    // Mask
    return _mm256_and_si256(_mm512_cvtepi32_epi16(main), mask);
}