//
// Created by harper on 3/21/18.
//

#ifndef SIMDSCAN_DECODER_H
#define SIMDSCAN_DECODER_H


#include <cstdint>

class Decoder {
public:
    Decoder();

    virtual ~Decoder();

    virtual void decode(int *input, uint64_t size, int *output)=0;
};


#endif //SIMDSCAN_DECODER_H
