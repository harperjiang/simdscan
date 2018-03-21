//
// Created by harper on 3/20/18.
//

#include "LaneLoader64.h"

#define LANE_SIZE 64
#define BYTE_SIZE 8

LaneLoader64::LaneLoader64(int es) {
    this->entrySize = es;

    // Given an offset, position of next 3 128-bit lane
    nextPos = new uint8_t[24];
    // Given an offset, position of next 512-bit lane
    nextIdx = new uint8_t[8];
    // Given an offset, offset of next 512-bit lane
    nextOff = new uint8_t[8];
    // How many entries are loaded
    counters = new uint8_t[8];

    shiftInst = (__m512i *) aligned_alloc(64, 8 * 64);
    shuffleInst = (__m512i *) aligned_alloc(64, 8 * 64);

    __m128i *shuffleBuffer = (__m128i *) aligned_alloc(16, 16 * 8);
    __m128i *shiftBuffer = (__m128i *) aligned_alloc(16, 16 * 8);
    uint64_t *shuffleDataBuffer = (uint64_t *) aligned_alloc(16, 16);
    uint64_t *shiftDataBuffer = (uint64_t *) aligned_alloc(16, 16);

    uint32_t countInLane[8][2];
    uint32_t laneOffset[2];
    // Compute shuffle and shift instructions
    for (int offset = 0; offset < 8; offset++) {
        uint32_t currentOffset = offset;
        uint32_t currentByte = 0;
        for (int idx = 0; idx < 2; idx++) {
            shiftDataBuffer[idx] = currentOffset;
            countInLane[offset][idx] = (LANE_SIZE - currentOffset) / entrySize;
            uint32_t bitsInLane = currentOffset + countInLane[offset][idx] * entrySize;
            shuffleDataBuffer[idx] = 0;
            uint64_t start = currentOffset / 8;

            for (uint32_t bi = 0; bi < 8; bi++) {
                if (bi * 8 + currentOffset < bitsInLane) {
                    shuffleDataBuffer[idx] |= (currentByte + bi) << bi * 8;
                } else {
                    shuffleDataBuffer[idx] |= 0xffL << bi * 8;
                }
            }
            currentOffset = (currentOffset + entrySize * countInLane[offset][idx]) % 8;
            currentByte += (currentOffset + entrySize * countInLane[offset][idx]) / 8;
        }
        shuffleBuffer[offset] = _mm_load_si128((__m128i *) shuffleDataBuffer);
        shiftBuffer[offset] = _mm_load_si128((__m128i *) shiftDataBuffer);
    }
    for (int offset = 0; offset < 8; offset++) {
        int numEntries = countInLane[offset][0] + countInLane[offset][1];
        for (int j = 0; j < 3; j++) {
            int newOffset = (offset + entrySize * numEntries) % 8;
            numEntries += countInLane[newOffset][0] + countInLane[newOffset][1];
            nextPos[offset * 3 + j] = newOffset;
        }
        nextIdx[offset] = (offset + numEntries * entrySize) / 8;
        nextOff[offset] = (offset + numEntries * entrySize) % 8;
        counters[offset] = numEntries;
    }


    // Combine them to make 512-bit shuffle and shift instructions
    for (int i = 0; i < 8; i++) {
        int high = (i + entrySize * 2) % 8;
        int higher = (high + entrySize * 2) % 8;
        int evenHigher = (higher + entrySize * 2) % 8;

        __m128i su0 = shuffleBuffer[i];
        __m128i su1 = shuffleBuffer[high];
        __m128i su2 = shuffleBuffer[higher];
        __m128i su3 = shuffleBuffer[evenHigher];

        __m512i shuffle = _mm512_castsi128_si512(su0);
        shuffle = _mm512_inserti64x2(shuffle, su1, 1);
        shuffle = _mm512_inserti64x2(shuffle, su2, 2);
        shuffle = _mm512_inserti64x2(shuffle, su3, 3);
        this->shuffleInst[i] = shuffle;

        __m128i sh0 = shiftBuffer[i];
        __m128i sh1 = shiftBuffer[high];
        __m128i sh2 = shiftBuffer[higher];
        __m128i sh3 = shiftBuffer[evenHigher];

        __m512i shift = _mm512_castsi128_si512(sh0);
        shift = _mm512_inserti64x2(shift, sh1, 1);
        shift = _mm512_inserti64x2(shift, sh2, 2);
        shift = _mm512_inserti64x2(shift, sh3, 3);
        this->shiftInst[i] = shift;
    }

    free(shuffleDataBuffer);
    free(shiftDataBuffer);
    free(shuffleBuffer);
    free(shiftBuffer);
}

LaneLoader64::~LaneLoader64() {
    free(shiftInst);
    free(shuffleInst);
    delete[] nextPos;
    delete[] nextIdx;
    delete[] nextOff;
    delete[] counters;
}

__m512i LaneLoader64::load(uint8_t *data, uint8_t offset, uint32_t *newidx, uint32_t *newoff, uint32_t *counter) {
    // Load 4 128 bit into a 512 bit register
    __m256i lower = _mm256_loadu2_m128i((__m128i *) (data + nextPos[offset * 3]), (__m128i *) data);
    __m256i higher = _mm256_loadu2_m128i((__m128i *) (data + nextPos[offset * 3 + 2]),
                                         (__m128i *) (data + nextPos[offset * 3 + 1]));
    // Get a single 512 bit
    __m512i main = _mm512_castsi256_si512(lower);
    main = _mm512_inserti64x4(main, higher, 1);

    // Shuffle
    __m512i shuffle = _mm512_shuffle_epi8(main, shuffleInst[offset]);
    // Shift
    __m512i shift = _mm512_srlv_epi64(shuffle, shiftInst[offset]);

    *newidx = nextIdx[offset];
    *newoff = nextOff[offset];
    *counter = counters[offset];

    return shift;
}