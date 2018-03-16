//
// Created by harper on 3/15/18.
//

#ifndef SIMDSCAN_LARGE16UNPACKER_H
#define SIMDSCAN_LARGE16UNPACKER_H


#include "Unpacker.h"

class Large16Unpacker : public Unpacker {
private:
    uint32_t entrySize;

    uint8_t *nextPos;

    __m512i *shuffleInst;
    __m512i *shiftInst;
    __m256i *mask;
public:
    Large16Unpacker(uint32_t es);

    virtual ~Large16Unpacker();

    __m256i unpack(uint8_t *data, uint8_t offset);
};


#endif //SIMDSCAN_LARGE16UNPACKER_H
