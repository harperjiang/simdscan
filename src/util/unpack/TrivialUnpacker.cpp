//
// Created by harper on 3/16/18.
//

#include "TrivialUnpacker.h"

TrivialUnpacker::TrivialUnpacker() {

}

TrivialUnpacker::~TrivialUnpacker() {

}

__m256i TrivialUnpacker::unpack(uint8_t *data, uint8_t offset) {
    return _mm256_loadu_si256((__m256i*)data);
}