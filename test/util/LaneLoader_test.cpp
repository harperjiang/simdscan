//
// Created by harper on 3/20/18.
//

#include <gtest/gtest.h>
#include "../../src/util/ll/LaneLoader.h"
#include "../../src/util/ll/LaneLoader64.h"

static uint64_t mm512_extract_epi64(__m512i data, int index);

TEST(LaneLoader64, load) {

    int *input = new int[100];
    srand(time(NULL));
    for (int i = 0; i < 100; i++)
        input[i] = rand();
    uint8_t *bytes = (uint8_t *) input;
    for (int es = 3; es < 32; es++) {
        LaneLoader *loader = new LaneLoader64(es);
        for (int offset = 0; offset < 8; offset++) {
            uint32_t nextByte;
            uint32_t nextOff;
            uint32_t counter;

            __m512i loaded = loader->load((uint8_t *) input, offset, &nextByte, &nextOff, &counter);

            // First check loaded data in 8 lanes
            int laneoff = offset;
            int idx = 0;
            int entryCounter = 0;
            for (int i = 0; i < 8; i++) {
                uint64_t lane = mm512_extract_epi64(loaded, i);

                int entryInLane = (64 - laneoff) / es;
                entryCounter += entryInLane;
                int bitsInLane = entryInLane * es;
                uint64_t mask = (bitsInLane == 64) ? 0xFFFFFFFF : (1 << bitsInLane) - 1;

                uint64_t expected = ((uint64_t *) (bytes + idx))[0] >> laneoff;

                ASSERT_EQ(expected & mask, lane & mask) << es << "," << offset << ", Lane " << i;

                idx += (laneoff + bitsInLane) / 8;
                laneoff = (laneoff + bitsInLane) % 8;
            }
            ASSERT_EQ(nextByte, idx) << es << "," << offset << ", Next Idx";
            ASSERT_EQ(nextOff, laneoff) << es << "," << offset << ", Next Off";
            ASSERT_EQ(entryCounter, counter) << es << "," << offset << ", Counter";
        }
    }
}

uint64_t mm256_extract_epi64(__m256i input, int index) {
    switch (index) {
        case 0:
            return _mm256_extract_epi64(input, 0);
        case 1:
            return _mm256_extract_epi64(input, 1);
        case 2:
            return _mm256_extract_epi64(input, 2);
        case 3:
            return _mm256_extract_epi64(input, 3);
        default:
            return 0;
    }
}

uint64_t mm512_extract_epi64(__m512i data, int index) {
    int idx256 = index / 4;
    int off256 = index % 4;

    __m256i lane;
    switch (idx256) {
        case 0:
            lane = _mm512_extracti64x4_epi64(data, 0);
            break;
        case 1:
            lane = _mm512_extracti64x4_epi64(data, 1);
            break;
        default:
            break;
    }
    return mm256_extract_epi64(lane, off256);
}
