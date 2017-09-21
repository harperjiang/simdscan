/*
 * WillhalmScanner.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include <immintrin.h>
#include "WillhalmScanner256.h"
#include "../util/template.h"
#include <assert.h>
#include <sys/param.h>

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
        return _mm256_sllv_epi64(
                _mm256_srlv_epi64(data, _mm256_setr_epi64x(right[0], right[1], right[2], right[3])),
                _mm256_setr_epi64x(-left[0], -left[1], -left[2], -left[3]));
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
    }
    for (int i = 0; i < 8; i++) {
        if (byteStop[i] - byteStart[i] >= 4) {
            return 1;
        }
    }
    return 0;
}

// Use 64-bit lane to shuffle numbers
__m256i shuffle64Lane256(__m256i data, int offset, int entrySize, __m256i *shift, __m256i *mask) {
    int entryByteOffset[STEP];
    for (int i = 0; i < STEP; i++) {
        entryByteOffset[i] = (offset + i * entrySize) % BYTE_LEN;
    }
    int shuffle1Idx[BYTE_IN_SIMD];
    int shuffle2Idx[BYTE_IN_SIMD];

    for (int i = 0; i < BYTE_IN_SIMD; i++) {
        shuffle1Idx[i] = 0xff;
        shuffle2Idx[i] = 0xff;
    }
    for (int j = 0; j < STEP / 2; j++) {
        int start1 = (offset + j * entrySize) / BYTE_LEN;
        int stop1 = (offset + (j + 1) * entrySize - 1) / BYTE_LEN;
        for (int i = 0; i <= stop1 - start1; i++) {
            if (start1 + i < BYTE_IN_SIMD) {
                shuffle1Idx[j * 8 + i] = start1 + i;
            }
        }
        int start2 = (offset + (4 + j) * entrySize) / BYTE_LEN;
        int stop2 = (offset + (5 + j) * entrySize - 1) / BYTE_LEN;
        for (int i = 0; i < stop2 - start2; i++) {
            if (start2 + i < BYTE_IN_SIMD) {
                shuffle2Idx[j * 8 + i] = start2 + i;
            }
        }
    }

    *mask = _mm256_set1_epi32((1 << entrySize) - 1);
    __m256i shuffle1 = _mm256_maskz_shuffle_epi8(0xffffffff, data, mm256_setr_epi8(shuffle1Idx));
    __m256i shuffle2 = _mm256_maskz_shuffle_epi8(0xffffffff, data, mm256_setr_epi8(shuffle2Idx));

    __m256i shift1 = _mm256_srlv_epi64(shuffle1,
                                       _mm256_setr_epi64x(entryByteOffset[0], entryByteOffset[1], entryByteOffset[2],
                                                          entryByteOffset[3]));
    __m256i shift2 = _mm256_srlv_epi64(shuffle2,
                                       _mm256_setr_epi64x(entryByteOffset[4], entryByteOffset[5], entryByteOffset[6],
                                                          entryByteOffset[7]));
    *shift = _mm256_set1_epi32(0);

    __m256i combined = _mm256_setr_epi32(_mm256_extract_epi32(shift1, 0), _mm256_extract_epi32(shift1, 2),
                                         _mm256_extract_epi32(shift1, 4), _mm256_extract_epi32(shift1, 6),
                                         _mm256_extract_epi32(shift2, 0), _mm256_extract_epi32(shift2, 2),
                                         _mm256_extract_epi32(shift2, 4), _mm256_extract_epi32(shift2, 6));

    return _mm256_and_si256(*mask, combined);
}

__m256i shuffleShift32Lane256(__m256i data, int offset, int entrySize, __m256i *shift, __m256i *mask) {
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
    __m256i shuffled = _mm256_maskz_shuffle_epi8(0xffffffff, data, shuffle);

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

__m256i shuffle32Lane256(__m256i data, int offset, int entrySize, __m256i *shift, __m256i *mask) {
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

    return _mm256_and_si256(_mm256_maskz_shuffle_epi8(0xffffffff, data, shuffle), *mask);
}

__m256i shuffle256(__m256i data, int offset, int entrySize, __m256i *shift, __m256i *mask) {
    int largeEntry = checkLargeEntry256(offset, entrySize);

    if (entrySize >= 26 && largeEntry >= 1) {
        // some entry cross 5 bytes
        if (entrySize > 29 && largeEntry == 2) {
            // Two entries cross 9 bytes, need to do each entry in a 64 bit lane.
            return shuffle64Lane256(data, offset, entrySize, shift, mask);
        } else {
            // both adjacent two entries are less than 9 bytes but single is more than 5 bytes
            return shuffleShift32Lane256(data, offset, entrySize, shift, mask);
        }
    } else {
        // Each entry is in 32-bit lane
        return shuffle32Lane256(data, offset, entrySize, shift, mask);
    }
}


WillhalmScanner256::WillhalmScanner256(int bitSize, bool aligned) {
    assert(entrySize < INT_LEN && entrySize > 0);
    this->entrySize = entrySize;
    this->data = NULL;
    this->length = 0;
    this->aligned = aligned;
    this->p = NULL;
    this->dest = NULL;
    this->destCounter = 0;
    this->bufferCounter = 0;
}

WillhalmScanner256::~WillhalmScanner256() {
    // TODO Auto-generated destructor stub
}

void WillhalmScanner256::writeToDest(__m256i result, int count) {
    for (int i = 0; i < count; i++)
        writeToDest(mm256_extract_epi32(result, i));
}

void WillhalmScanner256::writeToDest(int result) {
    this->buffer[bufferCounter++] = result;
    if (bufferCounter == 8) {
        __m256i write = _mm256_setr_epi32(buffer[0], buffer[1], buffer[2],
                                          buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
        _mm256_stream_si256((__m256i *) (dest + destCounter), write);
        bufferCounter = 0;
        destCounter += 8;
    }
}

void WillhalmScanner256::scan(int *data, uint64_t length, int *dest, Predicate *p) {
    this->data = data;
    this->length = length;
    this->p = p;
    this->dest = dest;

    this->destCounter = 0;
    this->bufferCounter = 0;

    if (aligned) {
        scanAligned();
    } else {
        scanUnaligned();
    }
}

void WillhalmScanner256::scanUnaligned() {
    uint64_t entryCounter = 0;
    int parallel = SIMD_LEN / INT_LEN;

    void *byteData = (void *) data;
    int byteOffset = 0;
    int bitOffset = 0;

    __m256i val1 = _mm256_set1_epi32(p->getVal1());
    __m256i val2 = _mm256_set1_epi32(p->getVal2());

    while (entryCounter < length) {
        __m256i current = _mm256_loadu_si256((__m256i *) (byteData + byteOffset));

        __m256i mask;
        __m256i shift;
        int numEntry = (SIMD_LEN - bitOffset) / entrySize;
        int numGroup = numEntry / parallel + (numEntry % parallel ? 1 : 0);
        int dataOffset = bitOffset;

        for (int i = 0; i < numGroup; i++) {
            int numEntryInGroup = numEntry - i * parallel;
            if (numEntryInGroup > parallel)
                numEntryInGroup = parallel;

            __m256i shuffled = shuffle256(current, dataOffset, entrySize, &shift,
                                          &mask);
            dataOffset += entrySize * STEP;
            // Compare the aligned data with predicate
            __m256i result;
            switch (p->getOpr()) {
                case opr_eq:
                case opr_neq:
                    result = _mm256_cmpeq_epi32(shuffled, _mm256_sllv_epi32(val1, shift));
                    break;
                case opr_in: {
                    __m256i v1shift = _mm256_sllv_epi32(val1, shift);
                    __m256i v2shift = _mm256_sllv_epi32(val2, shift);
                    __m256i lower = _mm256_cmpgt_epi32(shuffled, v1shift);
                    __m256i upper = _mm256_cmpgt_epi32(shuffled, v2shift);
                    __m256i leq = _mm256_cmpeq_epi32(shuffled, v1shift);
                    __m256i ueq = _mm256_cmpeq_epi32(shuffled, v2shift);
                    result = _mm256_or_si256(_mm256_xor_si256(lower, upper),
                                             _mm256_or_si256(leq, ueq));
                }
                    break;
                default:
                    break;
            }
            writeToDest(result, numEntryInGroup);
        }

        entryCounter += numEntry;

        // Compute next byte offset
        int bitAdvance = (bitOffset + numEntry * entrySize);
        int byteAdvance = bitAdvance / BYTE_LEN;
        byteOffset += byteAdvance;
        bitOffset = bitAdvance % BYTE_LEN;
    }
}

void WillhalmScanner256::compareAndWrite(__m256i shuffled, __m256i mask, __m256i shift, __m256i val1, __m256i val2,
                                         int count) {
    __m256i shifted = _mm256_srlv_epi32(shuffled, shift);
    __m256i result;
    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
            result = _mm256_cmpeq_epi32(shifted, val1);
            break;
        case opr_in: {
            __m256i lower = _mm256_cmpgt_epi32(shifted, val1);
            __m256i upper = _mm256_cmpgt_epi32(shifted, val2);
            __m256i leq = _mm256_cmpeq_epi32(shifted, val1);
            __m256i ueq = _mm256_cmpeq_epi32(shifted, val2);
            result = _mm256_or_si256(_mm256_xor_si256(lower, upper),
                                     _mm256_or_si256(leq, ueq));
        }
            break;
        default:
            break;
    }
    writeToDest(result, count);
}

void WillhalmScanner256::scanAligned() {
    __m256i *mdata = (__m256i *) data;

    __m256i val1 = _mm256_set1_epi32(p->getVal1());
    __m256i val2 = _mm256_set1_epi32(p->getVal2());

    __m256i prev = _mm256_set1_epi32(0);
    __m256i current;

    uint64_t laneCounter = 0;
    uint64_t numLane = length * entrySize / SIMD_LEN
                       + (length * entrySize % SIMD_LEN ? 1 : 0);
    int offset = 0;

    while (laneCounter < numLane) {
        current = _mm256_stream_load_si256(mdata + laneCounter);
        __m256i mask, shift;

        // Process cross-boundary entry
        if (offset) {
            // Shift prev and current to align
            int inprev = entrySize - offset;
            int inprevByte = inprev / BYTE_LEN + (inprev % BYTE_LEN ? 1 : 0);
            int alignByte = SIMD_LEN / BYTE_LEN - inprevByte;
            __m256i aligned = mm256_alignr_epi8(current, prev, alignByte);

            // Read entries from aligned lane
            int alignedOffset = BYTE_LEN - inprev % BYTE_LEN;

            int numEntry = MIN(STEP, ((SIMD_LEN - alignedOffset) / entrySize));
            __m256i shuffled = shuffle256(aligned, alignedOffset, entrySize, &shift, &mask);

            compareAndWrite(shuffled, mask, shift, val1, val2, numEntry);
            // Compute the offset in current
            offset += (numEntry - 1) * entrySize;
        }

        while (SIMD_LEN - offset >= entrySize) {
            int numEntry = MIN(STEP, (SIMD_LEN - offset) / entrySize);
            __m256i shuffled = shuffle256(current, offset, entrySize, &shift,
                                          &mask);
            offset += numEntry * entrySize;
            compareAndWrite(shuffled, mask, shift, val1, val2, numEntry);
        }

        offset = (offset >= SIMD_LEN) ? 0 : entrySize - SIMD_LEN + offset;
        laneCounter += 1;

        // Store last data in prev
        prev = current;
    }

}
