//
// Created by harper on 3/20/18.
//

#ifndef SIMDSCAN_LARGEUNPACKER128_H
#define SIMDSCAN_LARGEUNPACKER128_H


#include "Unpacker128.h"

class LargeUnpacker128: public Unpacker128 {
private:
    uint32_t entrySize;

public:
    LargeUnpacker128(uint32_t);
    virtual ~LargeUnpacker128();

    __m128i unpack(uint8_t *data, uint8_t offset);
};


#endif //SIMDSCAN_LARGEUNPACKER128_H
