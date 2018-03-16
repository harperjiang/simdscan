//
// Created by harper on 3/15/18.
//

#include "Small16Unpacker.h"

Small16Unpacker::Small16Unpacker(int es) {
    this->entrySize = es;
    assert(es <= 8);

    this->shuffleInst = (__m256i *) aligned_alloc(32, 32 * 8);
    this->shiftInst = (__m256i *) aligned_alloc(32, 32 * 8);
    this->mask = _mm256_set1_epi16((1 << entrySize) - 1);

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
        int higher = (i + entrySize * 8) % 8;
        __m256i shuffle = _mm256_castsi128_si256(shuffleBuffer[higher]);
        shuffle = _mm256_inserti128_si256(shuffle, shuffleBuffer[i], 1);
        this->shuffleInst[i] = shuffle;
        __m256i shift = _mm256_castsi128_si256(shiftBuffer[higher]);
        shift = _mm256_inserti128_si256(shift, shiftBuffer[i], 1);
        this->shiftInst[i] = shift;
    }

    free(shuffleDataBuffer);
    free(shiftDataBuffer);
    free(shuffleBuffer);
    free(shiftBuffer);
}

Small16Unpacker::~Small16Unpacker() {
    free(this->shuffleInst);
    free(this->shiftInst);
}

__m256i Small16Unpacker::unpack(uint8_t *data, uint8_t offset) {

    // Load two 128 bits, each contains 8 entries as a 256-bit
    uint32_t bitadv = entrySize * 8 + offset;

    __m256i main = _mm256_loadu2_m128i((__m128i *) (data + bitadv / 8), (__m128i *) data);
//    __m256i main = _mm256_inserti128_si256(_mm256_castsi128_si256(_mm_loadu_si128((__m128i *) data)),
//                                           _mm_loadu_si128((__m128i *) (data + bitadv / 8)), 1);

    // Shuffle bytes
    main = _mm256_shuffle_epi8(main, shuffleInst[offset]);
    // Shift bits
    main = _mm256_sllv_epi16(main, shiftInst[offset]);
    // Mask
    return _mm256_and_si256(main, mask);
}