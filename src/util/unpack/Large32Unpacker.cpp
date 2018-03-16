//
// Created by harper on 3/16/18.
//

#include <assert.h>
#include "Large32Unpacker.h"

Large32Unpacker::Large32Unpacker(uint32_t es) {
    this->entrySize = es;
    assert(es >= 26 && es < 32);

    nextPos = new uint8_t[24];

    shiftInst = (__m512i *) aligned_alloc(64, 8 * 64);
    shuffleInst = (__m512i *) aligned_alloc(64, 8 * 64);

    mask = (__m256i *) aligned_alloc(32, 32);
    *mask = _mm256_set1_epi32((1 << entrySize) - 1);

    __m128i *shuffleBuffer = (__m128i *) aligned_alloc(16, 16 * 8);
    __m128i *shiftBuffer = (__m128i *) aligned_alloc(16, 16 * 8);
    uint64_t *shuffleDataBuffer = (uint64_t *) aligned_alloc(16, 16);
    uint64_t *shiftDataBuffer = (uint64_t *) aligned_alloc(16, 16);

    // Compute shuffle and shift instructions
    for (int offset = 0; offset < 8; offset++) {
        for (int idx = 0; idx < 2; idx++) {
            uint32_t entryoff = offset + entrySize * idx;
            shiftDataBuffer[idx] = entryoff % 8;

            uint8_t round = (shiftDataBuffer[idx] + entrySize) / 8;
            shuffleDataBuffer[idx] = 0;
            int start = entryoff / 8;
            for (int bi = 0; bi <= round; bi++) {
                shuffleDataBuffer[idx] |= (start + bi) << bi * 8;
            }
            for (int bi = round + 1; bi < 8; bi++) {
                shuffleDataBuffer[idx] |= 0xff << bi * 8;
            }
        }
        for (int j = 0; j < 3; j++) {
            nextPos[offset * 3 + j] = (offset + entrySize * 2 * (j + 1)) / 8;
        }

        shuffleBuffer[offset] = _mm_load_si128((__m128i *) shuffleDataBuffer);
        shiftBuffer[offset] = _mm_load_si128((__m128i *) shiftDataBuffer);
    }

    // Combine them to make 512-bit shuffle and shift instructions
    for (int i = 0; i < 8; i++) {
        int high = (i + entrySize * 2) % 8;
        int higher = (high + entrySize * 2) % 8;
        int evenHigher = (higher + entrySize * 2) % 8;

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

Large32Unpacker::~Large32Unpacker() {
    free(shiftInst);
    free(shuffleInst);
    free(mask);
    delete[] nextPos;
}

__m256i Large32Unpacker::unpack(uint8_t *data, uint8_t offset) {
    // Load 4 128 bit into a 512 bit register
    __m256i lower = _mm256_loadu2_m128i((__m128i *) (data + nextPos[offset * 3]), (__m128i *) data);
    __m256i higher = _mm256_loadu2_m128i((__m128i *) (data + nextPos[offset * 3 + 1]),
                                         (__m128i *) (data + nextPos[offset * 3 + 2]));
    // Get a single 512 bit
    __m512i main = _mm512_castsi256_si512(lower);
    main = _mm512_inserti64x4(main, higher, 1);

    // Shuffle
    main = _mm512_shuffle_epi8(main, shuffleInst[offset]);
    // Shift
    main = _mm512_srlv_epi32(main, shiftInst[offset]);
    // Mask
    return _mm256_and_si256(_mm512_cvtepi64_epi32(main), *mask);
}