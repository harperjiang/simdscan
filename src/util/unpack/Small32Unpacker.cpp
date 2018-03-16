//
// Created by harper on 3/16/18.
//

#include "Small32Unpacker.h"

Small32Unpacker::Small32Unpacker(int es) {
    this->entrySize = es;
}

Small32Unpacker::~Small32Unpacker() {

}

__m256i Small32Unpacker::unpack(uint8_t *data, uint8_t offset) {

}