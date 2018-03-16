//
// Created by harper on 3/15/18.
//

#ifndef SIMDSCAN_LARGE16UNPACKER_H
#define SIMDSCAN_LARGE16UNPACKER_H


#include "Unpacker.h"

class Large16Unpacker : public Unpacker {
private:
    int entrySize;
    __m512i *shuffleInst;
    __m512i *shiftInst;
    __m256i mask;
public:
    Large16Unpacker(int es);

    virtual ~Large16Unpacker();

    __m256i unpack(uint8_t *data, uint8_t offset);
};


#endif //SIMDSCAN_LARGE16UNPACKER_H
