//
// Created by harper on 3/21/18.
//

#ifndef SIMDSCAN_BPDECODER_H
#define SIMDSCAN_BPDECODER_H


#include "Decoder.h"
#include "../util/unpack/u256/Unpacker.h"

class BPDecoder : public Decoder {
private:
    uint32_t entrySize;
    Unpacker *unpacker = NULL;
public:
    BPDecoder(uint32_t es);

    virtual ~BPDecoder();

    void decode(int *input, uint64_t size, int *output);
};


#endif //SIMDSCAN_BPDECODER_H
