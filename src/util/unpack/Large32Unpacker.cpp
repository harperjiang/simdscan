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

    mask = _mm256_set1_epi32((1 << entrySize) - 1);
}

Large32Unpacker::~Large32Unpacker() {
    free(shiftInst);
    free(shuffleInst);
    delete[] nextPos;
}

__m256i Large32Unpacker::unpack(uint8_t *data, uint8_t offset) {
    // Load 4 128 bit into a 512 bit register
    __m256i lower = _mm256_loadu2_m128i((__m128i *) (data + nextPos[offset]), (__m128i *) data);
    __m256i higher = _mm256_loadu2_m128i((__m128i *) (data + nextPos[offset + 1]),
                                         (__m128i *) (data + nextPos[offset + 2]));
    // Get a single 512 bit
    __m512i main = _mm512_castsi256_si512(lower);
    main = _mm512_inserti64x4(main, higher, 1);

    // Shuffle
    main = _mm256_shuffle_epi8(main, shuffleInst[offset]);
    // Shift
    main = _mm256_sllv_epi32(main, shiftInst[offset]);
    // Mask
    return _mm256_and_si256(main, mask);
}