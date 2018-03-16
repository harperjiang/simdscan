//
// Created by harper on 3/15/18.
//

#include "Small16Unpacker.h"

Small16Unpacker::Small16Unpacker(uint32_t es) {
    this->entrySize = es;
    assert(es <= 8);

    this->shuffleInst = (__m256i *) aligned_alloc(32, 32);
    this->shiftInst = (__m256i *) aligned_alloc(32, 32);
    this->mask = (__m256i *) aligned_alloc(32, 32);
    *mask = _mm256_set1_epi16((1 << entrySize) - 1);

    uint16_t *shuffleDataBuffer = (uint16_t *) aligned_alloc(16, 16);
    uint16_t *shiftDataBuffer = (uint16_t *) aligned_alloc(16, 16);
    // Compute shuffle and shift instructions
    for (int idx = 0; idx < 8; idx++) {
        uint32_t entryoff = entrySize * idx;
        shuffleDataBuffer[idx] = entryoff / 8;
        shiftDataBuffer[idx] = entryoff % 8;
        if (shiftDataBuffer[idx] + entrySize > 8) {
            shuffleDataBuffer[idx] |= (entryoff / 8 + 1) << 8;
        } else {
            shuffleDataBuffer[idx] |= 0xff << 8;
        }
    }
    __m128i shuffleBuffer = _mm_load_si128((__m128i *) shuffleDataBuffer);
    __m128i shiftBuffer = _mm_load_si128((__m128i *) shiftDataBuffer);

    // Combine them to make 256-bit shuffle and shift instructions
    *this->shuffleInst = _mm256_broadcastsi128_si256(shuffleBuffer);
    *this->shiftInst = _mm256_broadcastsi128_si256(shiftBuffer);

    free(shuffleDataBuffer);
    free(shiftDataBuffer);
}

Small16Unpacker::~Small16Unpacker() {
    free(this->shuffleInst);
    free(this->shiftInst);
    free(this->mask);
}

__m256i Small16Unpacker::unpack(uint8_t *data, uint8_t offset) {
    // other offset should not appear as we read 8 entries each time
    assert(offset == 0);
    // Load two 128 bits, each contains 8 entries as a 256-bit
    __m256i main = _mm256_loadu2_m128i((__m128i *) (data + entrySize), (__m128i *) data);

    // Shuffle bytes
    __m256i shuffle = _mm256_shuffle_epi8(main, *shuffleInst);
    // Shift bits
    __m256i shift = _mm256_sllv_epi16(shuffle, *shiftInst);
    // Mask
    return _mm256_and_si256(shift, *mask);
}