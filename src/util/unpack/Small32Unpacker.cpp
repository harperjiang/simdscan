//
// Created by harper on 3/16/18.
//

#include <assert.h>
#include "Small32Unpacker.h"

Small32Unpacker::Small32Unpacker(uint32_t es) {
    this->entrySize = es;
    assert(es > 16 && es < 26);

    this->nextPos = new uint8_t[8];

    this->shuffleInst = (__m256i *) aligned_alloc(32, 32 * 8);
    this->shiftInst = (__m256i *) aligned_alloc(32, 32 * 8);

    this->mask = (__m256i *) aligned_alloc(32, 32);
    *mask = _mm256_set1_epi32((1 << es) - 1);

    __m128i *shuffleBuffer = (__m128i *) aligned_alloc(16, 16 * 8);
    __m128i *shiftBuffer = (__m128i *) aligned_alloc(16, 16 * 8);
    uint32_t *shuffleDataBuffer = (uint32_t *) aligned_alloc(16, 16);
    uint32_t *shiftDataBuffer = (uint32_t *) aligned_alloc(16, 16);

    // Compute shuffle and shift instructions
    for (int offset = 0; offset < 8; offset++) {
        for (int idx = 0; idx < 4; idx++) {
            uint32_t entryoff = offset + entrySize * idx;
            shiftDataBuffer[idx] = entryoff % 8;

            uint8_t round = (shiftDataBuffer[idx] + entrySize) / 8;
            shuffleDataBuffer[idx] = 0;
            int start = entryoff / 8;
            for (int bi = 0; bi < round; bi++) {
                shuffleDataBuffer[idx] |= (start + bi) << bi * 8;
            }
            for (int bi = round; bi < 4; bi++) {
                shuffleDataBuffer[idx] |= 0xff << bi * 8;
            }
        }
        nextPos[offset] = (offset + entrySize * 4) / 8;

        shuffleBuffer[offset] = _mm_load_si128((__m128i *) shuffleDataBuffer);
        shiftBuffer[offset] = _mm_load_si128((__m128i *) shiftDataBuffer);
    }

    // Combine them to make 256-bit shuffle and shift instructions
    for (int i = 0; i < 8; i++) {
        uint32_t higher = (i + entrySize * 8) % 8;
        this->shuffleInst[i] = _mm256_set_m128i(shuffleBuffer[higher], shuffleBuffer[i]);
        this->shiftInst[i] = _mm256_set_m128i(shiftBuffer[higher], shiftBuffer[i]);
    }

    free(shuffleDataBuffer);
    free(shiftDataBuffer);
    free(shuffleBuffer);
    free(shiftBuffer);

}

Small32Unpacker::~Small32Unpacker() {
    delete[] this->nextPos;
    free(shuffleInst);
    free(mask);
    free(shiftInst);
}

__m256i Small32Unpacker::unpack(uint8_t *data, uint8_t offset) {
    // Load data into 2 128 buffer and combine as 256
    __m256i main = _mm256_loadu2_m128i((__m128i *) (data + nextPos[offset]), (__m128i *) data);
    // Shuffle
    main = _mm256_shuffle_epi8(main, shuffleInst[offset]);
    // Shift
    main = _mm256_sllv_epi32(main, shiftInst[offset]);
    // Mask
    return _mm256_and_si256(main, *mask);
}