/*
 * WillhalmScanner.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include <immintrin.h>
#include "WillhalmScanner256.h"
#include <assert.h>

#define INT_LEN 32
#define SIMD_LEN 256
#define BYTE_LEN 8
#define BYTE_IN_SIMD SIMD_LEN / BYTE_LEN
#define STEP SIMD_LEN / INT_LEN


__m256i mm256_setr_epi8(int *data) {
    return _mm256_setr_epi8(data[0], data[1], data[2], data[3], data[4], data[5],
                            data[6], data[7], data[8], data[9], data[10], data[11], data[12],
                            data[13], data[14], data[15], data[16], data[17], data[18], data[19], data[20], data[21],
                            data[22], data[23], data[24], data[25], data[26], data[27], data[28], data[29], data[30],
                            data[31]);
}

__m256i shift_64_256(__m256i data, int *s) {
    int left[] = {0, 0, 0, 0};
    int right[] = {0, 0, 0, 0};
    int leftCount = 0;
    int rightCount = 0;
    for (int i = 0; i < 4; i++) {
        if (s[i] >= 0) {
            rightCount++;
            right[i] = s[i];
        } else {
            leftCount++;
            left[i] = s[i];
        }
    }
    if (leftCount == 0) {
        return _mm256_srlv_epi64(data, _mm256_setr_epi64x(s[0], s[1], s[2], s[3]));
    } else if (rightCount == 0) {
        return _mm256_sllv_epi64(data, _mm256_setr_epi64x(-s[0], -s[1], -s[2], -s[3]));
    } else {
        return _mm_sllv_epi64(
                _mm_srlv_epi64(data, _mm_setr_epi32(right[0], right[1], right[2], right[3])),
                _mm_setr_epi32(-left[0], -left[1], -left[2], -left[3]));
    }
}

int checkLargeEntry256(int offset, int entrySize) {
    int byteStart[8];
    int byteStop[8];
    for (int i = 0; i < 8; i++) {
        byteStart[i] = (offset + entrySize * i) / BYTE_LEN;
        byteStop[i] = ((offset + entrySize * (i + 1)) - 1) / BYTE_LEN;
    }
    for (int i = 0; i < 8; i++) {
        if (i % 2 == 0 && byteStop[i + 1] - byteStart[i] >= 8)
            return 2;
        if (byteStop[i] - byteStart[i] >= 4) {
            return 1;
        }
    }
    return 0;
}

// Use 64-bit lane to shuffle numbers
__m256i shuffle64Lane(__m256i data, int offset, int entrySize, __m256i *shift, __m256i *mask) {
    int entryByteOffset[4];
    for (int i = 0; i < 4; i++) {
        entryByteOffset[i] = (offset + i * entrySize) % BYTE_LEN;
    }
    int shuffle1Idx[16];
    int shuffle2Idx[16];
    for (int i = 0; i < 16; i++) {
        shuffle1Idx[i] = 0xff;
        shuffle2Idx[i] = 0xff;
    }

    int start = offset / BYTE_LEN;
    int stop = (offset + entrySize - 1) / BYTE_LEN;
    for (int i = 0; i <= stop - start; i++) {
        if (start + i < BYTE_IN_SIMD) {
            shuffle1Idx[i] = start + i;
        }
    }
    start = (offset + entrySize) / BYTE_LEN;
    stop = (offset + 2 * entrySize - 1) / BYTE_LEN;
    for (int i = 0; i <= stop - start; i++) {
        if (start + i < BYTE_IN_SIMD) {
            shuffle1Idx[i + 8] = start + i;
        }
    }
    start = (offset + 2 * entrySize) / BYTE_LEN;
    stop = (offset + 3 * entrySize - 1) / BYTE_LEN;
    for (int i = 0; i <= stop - start; i++) {
        if (start + i < BYTE_IN_SIMD) {
            shuffle2Idx[i] = start + i;
        }
    }
    start = (offset + 3 * entrySize) / BYTE_LEN;
    stop = (offset + 4 * entrySize - 1) / BYTE_LEN;
    for (int i = 0; i <= stop - start; i++) {
        if (start + i < BYTE_IN_SIMD) {
            shuffle2Idx[i + 8] = start + i;
        }
    }

    *mask = _mm_set1_epi32((1 << entrySize) - 1);
    __m256i e12 = _mm_and_si256(_mm_srlv_epi64(
            _mm_shuffle_epi8(data, setr_epi8(shuffle1Idx)),
            _mm_setr_epi32(entryByteOffset[0], 0, entryByteOffset[1], 0)), *mask);
    __m256i e34 = _mm_and_si256(_mm_srlv_epi64(
            _mm_shuffle_epi8(data, setr_epi8(shuffle2Idx)),
            _mm_setr_epi32(entryByteOffset[2], 0, entryByteOffset[3], 0)), *mask);

    __m256i compose = _mm_setr_epi32(_mm_extract_epi32(e12, 0), _mm_extract_epi32(e12, 2), _mm_extract_epi32(e34, 0),
                                     _mm_extract_epi32(e34, 2));
    *shift = _mm_set1_epi32(0);
    return compose;
}

__m256i shuffleShift32Lane(__m256i data, int offset, int entrySize, __m256i *shift, __m256i *mask) {
    int shuffleIdx[BYTE_IN_SIMD];
    for (int i = 0; i < BYTE_IN_SIMD; i++) {
        shuffleIdx[i] = 0xff;
    }
    for (int j = 0; j < 3; j++) {
        int start = (offset + 2 * j * entrySize) / BYTE_LEN;
        int stop = (offset + (2 * j + 2) * entrySize - 1) / BYTE_LEN;
        for (int i = 0; i <= stop - start; i++) {
            if (start + i < BYTE_IN_SIMD) {
                shuffleIdx[8 * j + i] = start + i;
            }
        }
    }

    __m256i shuffle = mm256_setr_epi8(shuffleIdx);
    __m256i shuffled = _mm256_shuffle_epi8(data, shuffle);

    // shift for entries

    int s[4];
    for (int i = 0; i < 4; i++) {
        s[i] = (offset + 2 * i * entrySize) % BYTE_LEN + entrySize - INT_LEN;
    }

    *shift = _mm256_setr_epi32(INT_LEN - entrySize, 0, INT_LEN - entrySize, 0, INT_LEN - entrySize, 0,
                               INT_LEN - entrySize, 0);

    int lower = 0xffffffff << (INT_LEN - entrySize);
    int higher = (1 << entrySize) - 1;

    *mask = _mm256_setr_epi32(lower, higher, lower, higher, lower, higher, lower, higher);

    __m256i shifted = shift_64_256(shuffled, s);
    return _mm256_and_si256(shifted, *mask);
}

__m256i shuffle32Lane(__m256i data, int offset, int entrySize, __m256i *shift, __m256i *mask) {
    int shuffleIdx[BYTE_IN_SIMD];
    int maskIdx[STEP];
    int shiftIdx[STEP];
    for (int i = 0; i < BYTE_IN_SIMD; i++) {
        shuffleIdx[i] = 0xff;
    }
    // Each of the 32-bit entry
    for (int j = 0; j < STEP; j++) {
        int startByte = (offset + j * entrySize) / BYTE_LEN;
        int stopByte = (offset + (j + 1) * entrySize - 1) / BYTE_LEN;
        for (int k = 0; k <= stopByte - startByte; k++) {
            if (startByte + k < BYTE_IN_SIMD) {
                shuffleIdx[j * STEP + k] = startByte + k;
            }
        }
        int shift = (offset + j * entrySize) % BYTE_LEN;
        maskIdx[j] = ((1 << entrySize) - 1) << shift;
        shiftIdx[j] = shift;
    }

    __m256i shuffle = mm256_setr_epi8(shuffleIdx);
    *mask = _mm256_setr_epi32(maskIdx[0], maskIdx[1], maskIdx[2], maskIdx[3], maskIdx[4], maskIdx[5], maskIdx[6],
                              maskIdx[7]);
    *shift = _mm256_setr_epi32(shiftIdx[0], shiftIdx[1], shiftIdx[2], shiftIdx[3], maskIdx[4], maskIdx[5], maskIdx[6],
                               maskIdx[7]);

    return _mm256_and_si256(_mm256_shuffle_epi8(data, shuffle), *mask);
}

__m256i shuffle(__m256i data, int offset, int entrySize, __m256i *shift, __m256i *mask) {
    int largeEntry = checkLargeEntry256(offset, entrySize);

    if (entrySize >= 26 && largeEntry >= 1) {
        // some entry cross 5 bytes
        if (entrySize > 29 && largeEntry == 2) {
            // Two entries cross 9 bytes, need to do each entry in a 64 bit lane.
            return shuffle64Lane(data, offset, entrySize, shift, mask);
        } else {
            // both adjacent two entries are less than 9 bytes but single is more than 5 bytes
            return shuffleShift32Lane(data, offset, entrySize, shift, mask);
        }
    } else {
        // Each entry is in 32-bit lane
        return shuffle32Lane(data, offset, entrySize, shift, mask);
    }
}

int WillhalmScanner256::computeShift(int counter) {
    return 0;
}

WillhalmScanner256::WillhalmScanner256(int bitSize) {
    this->bitSize = bitSize;
}

WillhalmScanner256::~WillhalmScanner256() {
    // TODO Auto-generated destructor stub
}

void WillhalmScanner256::scan(int *data, int length, int *dest, Predicate *p) {
    int counter = 0;
    int step = SIMD_LEN / INT_LEN;
    // TODO For experimental purpose, assume data is aligned for now
    assert(length % step == 0);

    __m256i prev;
    __m256i current;
    while (counter < length) {
        current = _mm256_stream_load_si256((__m256i *) (data + counter));
        // Shift data to have 32 byte alignment
        int shift = computeShift(counter);
        if (shift != 0) {
//			current = _mm256_alignr_epi8(prev, current, shift);
        }
        // Shift data to have 16 byte alignment
        // Use shuffle to make 4-byte alignment


        // Use shuffle to make bit-alignment

        counter += step;
        // Store last data in prev
        prev = current;
    }
}

