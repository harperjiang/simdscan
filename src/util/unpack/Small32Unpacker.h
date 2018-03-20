//
// Created by harper on 3/16/18.
//

#ifndef SIMDSCAN_SMALL32UNPACKER_H
#define SIMDSCAN_SMALL32UNPACKER_H


#include "Unpacker.h"

class Small32Unpacker : public Unpacker128 {
private:
    uint32_t entrySize;

    uint8_t* nextPos;

    __m256i *shuffleInst;
    __m256i *shiftInst;
    __m256i *mask;
public:
    Small32Unpacker(uint32_t es);

    virtual ~Small32Unpacker();

    __m256i unpack(uint8_t *data, uint8_t offset);
};


#endif //SIMDSCAN_SMALL32UNPACKER_H
