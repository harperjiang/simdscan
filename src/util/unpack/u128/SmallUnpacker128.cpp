//
// Created by harper on 3/20/18.
//

#include "SmallUnpacker128.h"

SmallUnpacker128::SmallUnpacker128(uint32_t es) {
   this->entrySize = es;
}

SmallUnpacker128::~SmallUnpacker128() {

}

__m128i SmallUnpacker128::unpack(uint8_t *data, uint8_t offset) {

}