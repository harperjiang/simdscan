//
// Created by harper on 3/15/18.
//

#ifndef SIMDSCAN_UNPACKER_H
#define SIMDSCAN_UNPACKER_H


/**
 * Implementations of unpacking integer into 256-bit SIMD
 */
#include <immintrin.h>
#include <cstdint>

#define _mm256_loadu2_m128i(vh, vl) _mm256_insertf128_si256(_mm256_castsi128_si256(_mm_loadu_si128((vl))), _mm_loadu_si128(vh), 1)
#define _mm256_set_m128i(vh, vl)  _mm256_insertf128_si256(_mm256_castsi128_si256(vl), (vh), 1)

class Unpacker {

public:
    /**
     *
     * @param es entry size
     */
    Unpacker() {

    }

    virtual ~Unpacker() {

    }

    /**
     *
     * @param data byte buffer
     * @param offset offset in a byte
     * @return unpacked integer
     */
    virtual __m256i unpack(uint8_t *data, uint8_t offset) =0;
};


#endif //SIMDSCAN_UNPACKER_H
