//
// Created by harper on 3/15/18.
//

#ifndef SIMDSCAN_UNPACKER128_H
#define SIMDSCAN_UNPACKER128_H


/**
 * Implementations of unpacking integer into 256-bit SIMD
 */
#include <immintrin.h>
#include <cstdint>

class Unpacker128 {

public:
    /**
     *
     * @param es entry size
     */
    Unpacker128() {

    }

    virtual ~Unpacker128() {

    }

    /**
     *
     * @param data byte buffer
     * @param offset offset in a byte
     * @return unpacked integer
     */
    virtual __m128i unpack(uint8_t *data, uint8_t offset) =0;
};


#endif //SIMDSCAN_UNPACKER128_H
