//
// Created by harper on 3/21/18.
//

#include "BPDecoder.h"
#include "../util/unpack/u256/Small32Unpacker.h"
#include "../util/unpack/u256/Large32Unpacker.h"

BPDecoder::BPDecoder(uint32_t es) {
    this->entrySize = es;
    if (es < 26)
        this->unpacker = new Small32Unpacker(es);
    else
        this->unpacker = new Large32Unpacker(es);
}

BPDecoder::~BPDecoder() {
    delete this->unpacker;
}

void BPDecoder::decode(int *input, uint64_t size, int *output) {
    uint8_t *bytein = (uint8_t *) input;
    __m256i *simdout = (__m256i *) output;
    uint64_t round = size / 8 + (size % 8 ? 1 : 0);
    uint32_t byteOffset = 0;
    uint32_t offset = 0;
    uint64_t outputCounter = 0;
    for (uint64_t i = 0; i < round; i++) {
        __m256i unpacked = unpacker->unpack(bytein + byteOffset, offset);
        _mm256_stream_si256(simdout + outputCounter++, unpacked);

        uint64_t bitAdvance = offset + 8 * entrySize;
        byteOffset += bitAdvance / 8;
        offset = bitAdvance % 8;
    }
}