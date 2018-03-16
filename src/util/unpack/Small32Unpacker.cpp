//
// Created by harper on 3/16/18.
//

#include <assert.h>
#include "Small32Unpacker.h"

Small32Unpacker::Small32Unpacker(uint32_t es) {
    this->entrySize = es;
    assert(es > 16 && es < 26);

    this->nextPos = new uint8_t[8];

    for (int i = 0; i < 8; i++) {
        nextPos[i] = (i + entrySize * 4) / 8;
    }
}

Small32Unpacker::~Small32Unpacker() {
    delete[] this->nextPos;
}

__m256i Small32Unpacker::unpack(uint8_t *data, uint8_t offset) {
    // Load data into 2 128 buffer and combine as 256
    __m256i main = _mm256_loadu2_m128i((__m128i *) (data + nextPos[offset]), (__m128i *) data);
    // Shuffle
    main = _mm256_shuffle_epi8(main, shuffleInst[offset]);
    // Shift
    main = _mm256_sllv_epi32(main, shiftInst[offset]);
    // Mask
    return _mm256_and_si256(main, mask);
}