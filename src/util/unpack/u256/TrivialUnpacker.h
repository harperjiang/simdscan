//
// Created by harper on 3/16/18.
//

#ifndef SIMDSCAN_TRIVIALUNPACKER_H
#define SIMDSCAN_TRIVIALUNPACKER_H


#include "Unpacker.h"

class TrivialUnpacker: public Unpacker {
public:
    TrivialUnpacker();
    virtual ~TrivialUnpacker();


    __m256i unpack(uint8_t *data, uint8_t offset);
};


#endif //SIMDSCAN_TRIVIALUNPACKER_H
