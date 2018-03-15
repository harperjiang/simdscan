//
// Created by harper on 3/15/18.
//

#include "Large16Unpacker.h"

Large16Unpacker::Large16Unpacker(int es) {
    this->entrySize = es;

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
        shuffleBuffer[offset] = _mm_loadu_si128((__m128i *) shuffleDataBuffer);
        shiftBuffer[offset] = _mm_loadu_si128((__m128i *) shiftDataBuffer);
    }

    // Combine them to make 256-bit shuffle and shift instructions
    for (int i = 0; i < 8; i++) {
        int high = (i + entrySize * 8) % 8;
        int higher = (high + entrySize * 8) % 8;
        int evenHigher = (higher + entrySize * 8) % 8;

        __m128i su0 = shuffleBuffer[i];
        __m128i su1 = shuffleBuffer[high];
        __m128i su2 = shuffleBuffer[higher];
        __m128i su3 = shuffleBuffer[evenHigher];

        __m256i shuffle = _mm256_castsi128_si256(shuffleBuffer[higher]);
        shuffle = _mm256_inserti128_si256(shuffle, shuffleBuffer[i], 1);
        this->shuffleInst[i] = shuffle;

        __m128i sh0 = shuffleBuffer[i];
        __m128i sh1 = shuffleBuffer[high];
        __m128i sh2 = shuffleBuffer[higher];
        __m128i sh3 = shuffleBuffer[evenHigher];

        __m256i shift = _mm256_castsi128_si256(shiftBuffer[higher]);
        shift = _mm256_inserti128_si256(shift, shiftBuffer[i], 1);
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
}

__m256i Large16Unpacker::unpack(uint8_t *data, uint8_t offset) {

    // Load 4 128 bits, each contains 4 entries
    __m256i lower = _mm256_loadu_si256((__m256i *) data);
    uint8_t lowoff = offset;

    uint32_t bitadv = entrySize * 8 + offset;
    __m256i higher = _mm256_loadu_si256((__m256i *) data + bitadv / 8);
    uint8_t highoff = bitadv % 8;

    // Get a single 512 bit
    __m512 main = _mm512_castsi256_si512(lower);
    main = _mm512_inserti64x4(main, higher, 1);

    uint32_t index = highoff * 8 + lowoff;
    // Shuffle bytes
    main = _mm512_shuffle_epi8(main, shuffleInst[index]);
    // Shift bits
    main = _mm512_sllv_epi16(main, shiftInst[index]);
    // Mask
    return _mm512_and_si512(_mm512_cvtepi32_epi16(main), mask);
}