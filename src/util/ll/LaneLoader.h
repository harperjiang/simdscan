//
// Created by harper on 3/20/18.
//

#ifndef SIMDSCAN_LANELOADER_H
#define SIMDSCAN_LANELOADER_H

#include <immintrin.h>
#include <stdint.h>

class LaneLoader {
public:
    LaneLoader();

    virtual ~LaneLoader();

    virtual __m512i load(uint8_t *input, uint8_t offset, uint32_t *newidx, uint32_t *newoff, uint32_t *counter) = 0;
};


#endif //SIMDSCAN_LANELOADER_H
