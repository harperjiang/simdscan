//
// Created by harper on 3/16/18.
//

#ifndef SIMDSCAN_SMALL32UNPACKER_H
#define SIMDSCAN_SMALL32UNPACKER_H


#include "Unpacker.h"

class Small32Unpacker: public Unpacker {
private:
    int entrySize;
public:
    Small32Unpacker(int es);

    virtual ~Small32Unpacker();

    __m256i unpack(uint8_t *data, uint8_t offset);
};


#endif //SIMDSCAN_SMALL32UNPACKER_H
