//
// Created by harper on 3/20/18.
//

#ifndef SIMDSCAN_LANELOADER64_H
#define SIMDSCAN_LANELOADER64_H


#include "LaneLoader.h"

#define _mm256_loadu2_m128i(vh, vl) _mm256_insertf128_si256(_mm256_castsi128_si256(_mm_loadu_si128((vl))), _mm_loadu_si128(vh), 1)

class LaneLoader64 : public LaneLoader {
private:
    uint32_t entrySize;

    uint8_t *nextPos;
    uint8_t *nextIdx;
    uint8_t *nextOff;
    uint8_t *counters;
    __m512i *shiftInst;
    __m512i *shuffleInst;
public:
    LaneLoader64(int es);

    virtual ~LaneLoader64();

    __m512i load(uint8_t *, uint8_t, uint32_t *, uint32_t *, uint32_t *);
};


#endif //SIMDSCAN_LANELOADER64_H
