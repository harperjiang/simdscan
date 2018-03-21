//
// Created by harper on 3/20/18.
//

#ifndef SIMDSCAN_SMALLUNPACKER128_H
#define SIMDSCAN_SMALLUNPACKER128_H


#include "Unpacker128.h"

class SmallUnpacker128: public Unpacker128 {
private:
    uint32_t entrySize;
public:
    SmallUnpacker128(uint32_t);
    virtual ~SmallUnpacker128();

    __m128i unpack(uint8_t *data, uint8_t offset);
};


#endif //SIMDSCAN_SMALLUNPACKER128_H
