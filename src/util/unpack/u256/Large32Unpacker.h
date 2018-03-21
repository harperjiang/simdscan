//
// Created by harper on 3/16/18.
//

#ifndef SIMDSCAN_LARGE32UNPACKER_H
#define SIMDSCAN_LARGE32UNPACKER_H


#include "Unpacker.h"

class Large32Unpacker : public Unpacker {
private:
    uint32_t entrySize;

    uint8_t *nextPos;

    __m512i *shuffleInst;
    __m512i *shiftInst;

    __m256i *mask;
public:
    Large32Unpacker(uint32_t es);

    virtual ~Large32Unpacker();

    __m256i unpack(uint8_t *data, uint8_t offset);
};


#endif //SIMDSCAN_LARGE32UNPACKER_H
