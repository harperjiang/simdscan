/*
 * WillhalmScanner.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include <immintrin.h>
#include "WillhalmScanner128.h"
#include "../../util/template.h"
#include <cassert>
#include <sys/param.h>

#define INT_LEN 32
#define SIMD_LEN 128
#define BYTE_LEN 8
#define BYTE_IN_SIMD SIMD_LEN / BYTE_LEN
#define STEP SIMD_LEN / INT_LEN

__m128i setr_epi8(int *data) {
    return _mm_setr_epi8(data[0], data[1], data[2], data[3], data[4], data[5],
                         data[6], data[7], data[8], data[9], data[10], data[11], data[12],
                         data[13], data[14], data[15]);
}

// Shift number in 64 bit lane, positive means shift right, negative means shift left
__m128i shift_64(__m128i data, int s12, int s34) {
    if (s12 >= 0 && s34 >= 0) {
        return _mm_srlv_epi64(data, _mm_setr_epi32(s12, 0, s34, 0));
    } else if (s12 <= 0 && s34 <= 0) {
        return _mm_sllv_epi64(data, _mm_setr_epi32(-s12, 0, -s34, 0));
    } else if (s12 > 0) {
        return _mm_sllv_epi64(
                _mm_srlv_epi64(data, _mm_setr_epi32(s12, 0, 0, 0)),
                _mm_setr_epi32(0, 0, -s34, 0));
    } else {
        return _mm_srlv_epi64(
                _mm_sllv_epi64(data, _mm_setr_epi32(-s12, 0, 0, 0)),
                _mm_setr_epi32(0, 0, s34, 0));
    }
}

int checkLargeEntry(int offset, int entrySize) {
    int byteStart[4];
    int byteStop[4];
    for (int i = 0; i < 4; i++) {
        byteStart[i] = (offset + entrySize * i) / BYTE_LEN;
        byteStop[i] = ((offset + entrySize * (i + 1)) - 1) / BYTE_LEN;
    }
    if (byteStop[1] - byteStart[0] >= 8 || byteStop[3] - byteStart[2] >= 8)
        return 2;
    for (int i = 0; i < 4; i++) {
        if (byteStop[i] - byteStart[i] >= 4) {
            return 1;
        }
    }
    return 0;
}

// Use 64-bit lane to shuffle numbers
__m128i shuffle64Lane(__m128i data, int offset, int entrySize, __m128i *shift, __m128i *mask) {
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
    __m128i e12 = _mm_and_si128(_mm_srlv_epi64(
            _mm_shuffle_epi8(data, setr_epi8(shuffle1Idx)),
            _mm_setr_epi32(entryByteOffset[0], 0, entryByteOffset[1], 0)), *mask);
    __m128i e34 = _mm_and_si128(_mm_srlv_epi64(
            _mm_shuffle_epi8(data, setr_epi8(shuffle2Idx)),
            _mm_setr_epi32(entryByteOffset[2], 0, entryByteOffset[3], 0)), *mask);

    __m128i compose = _mm_setr_epi32(_mm_extract_epi32(e12, 0), _mm_extract_epi32(e12, 2), _mm_extract_epi32(e34, 0),
                                     _mm_extract_epi32(e34, 2));
    *shift = _mm_set1_epi32(0);
    return compose;
}

__m128i shuffleShift32Lane(__m128i data, int offset, int entrySize, __m128i *shift, __m128i *mask) {
    int shuffleIdx[16];
    for (int i = 0; i < 16; i++) {
        shuffleIdx[i] = 0xff;
    }

    int start = (offset) / BYTE_LEN;
    int stop = (offset + 2 * entrySize - 1) / BYTE_LEN;
    for (int i = 0; i <= stop - start; i++) {
        if (start + i < BYTE_IN_SIMD) {
            shuffleIdx[i] = start + i;
        }
    }
    start = (offset + 2 * entrySize) / BYTE_LEN;
    stop = (offset + 4 * entrySize - 1) / BYTE_LEN;
    for (int i = 0; i <= stop - start; i++) {
        if (start + i < BYTE_IN_SIMD) {
            shuffleIdx[8 + i] = start + i;
        }
    }

    __m128i shuffle = setr_epi8(shuffleIdx);
    __m128i shuffled = _mm_shuffle_epi8(data, shuffle);

    // shift for entry 1 and 2, 3 and 4
    int s12 = offset + entrySize - INT_LEN;
    int s34 = (offset + 2 * entrySize) % BYTE_LEN + entrySize - INT_LEN;

    *shift = _mm_setr_epi32(INT_LEN - entrySize, 0, INT_LEN - entrySize, 0);

    int lower = 0xffffffff << (INT_LEN - entrySize);
    int higher = (1 << entrySize) - 1;

    *mask = _mm_setr_epi32(lower, higher, lower, higher);

    __m128i shifted = shift_64(shuffled, s12, s34);
    return _mm_and_si128(shifted, *mask);
}

__m128i shuffle32Lane(__m128i data, int offset, int entrySize, __m128i *shift, __m128i *mask) {
    int shuffleIdx[16];
    int maskIdx[4];
    int shiftIdx[4];
    for (int i = 0; i < 16; i++) {
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

    __m128i shuffle = setr_epi8(shuffleIdx);
    *mask = _mm_setr_epi32(maskIdx[0], maskIdx[1], maskIdx[2], maskIdx[3]);
    *shift = _mm_setr_epi32(shiftIdx[0], shiftIdx[1], shiftIdx[2], shiftIdx[3]);

    return _mm_and_si128(_mm_shuffle_epi8(data, shuffle), *mask);
}

__m128i shuffle(__m128i data, int offset, int entrySize, __m128i *shift, __m128i *mask) {
    int largeEntry = checkLargeEntry(offset, entrySize);

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

WillhalmScanner128::WillhalmScanner128(int entrySize, bool aligned = false) {
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

WillhalmScanner128::~WillhalmScanner128() {
    // TODO Auto-generated destructor stub
}

void WillhalmScanner128::writeToDest(__m128i result, int count) {
    for (int i = 0; i < count; i++)
        writeToDest(mm_extract_epi32(result, i));
}

void WillhalmScanner128::writeToDest(int result) {
    this->buffer[bufferCounter++] = result;
    if (bufferCounter == 4) {
        __m128i write = _mm_setr_epi32(buffer[0], buffer[1], buffer[2],
                                       buffer[3]);
        _mm_stream_si128((__m128i *) (dest + destCounter), write);
        bufferCounter = 0;
        destCounter += 4;
    }
}

void WillhalmScanner128::scan(int *data, uint64_t length, int *dest, Predicate *p) {
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

void WillhalmScanner128::scanUnaligned() {
    uint64_t entryCounter = 0;
    int parallel = SIMD_LEN / INT_LEN;

    void *byteData = (void *) data;
    int byteOffset = 0;
    int bitOffset = 0;

    __m128i val1 = _mm_set1_epi32(p->getVal1());
    __m128i val2 = _mm_set1_epi32(p->getVal2());

    while (entryCounter < length) {
        __m128i current = _mm_loadu_si128((__m128i *) (byteData + byteOffset));

        __m128i mask;
        __m128i shift;
        int numEntry = (SIMD_LEN - bitOffset) / entrySize;
        int numGroup = numEntry / parallel + (numEntry % parallel ? 1 : 0);
        int dataOffset = bitOffset;

        for (int i = 0; i < numGroup; i++) {
            int numEntryInGroup = numEntry - i * parallel;
            if (numEntryInGroup > parallel)
                numEntryInGroup = parallel;

            __m128i shuffled = shuffle(current, dataOffset, entrySize, &shift,
                                       &mask);
            dataOffset += entrySize * STEP;
            // Compare the aligned data with predicate
            __m128i result;
            switch (p->getOpr()) {
                case opr_eq:
                case opr_neq:
                    result = _mm_cmpeq_epi32(shuffled, _mm_sllv_epi32(val1, shift));
                    break;
                case opr_in: {
                    __m128i v1shift = _mm_sllv_epi32(val1, shift);
                    __m128i v2shift = _mm_sllv_epi32(val2, shift);
                    __m128i lower = _mm_cmpgt_epi32(shuffled, v1shift);
                    __m128i upper = _mm_cmpgt_epi32(shuffled, v2shift);
                    __m128i leq = _mm_cmpeq_epi32(shuffled, v1shift);
                    __m128i ueq = _mm_cmpeq_epi32(shuffled, v2shift);
                    result = _mm_or_si128(_mm_xor_si128(lower, upper),
                                          _mm_or_si128(leq, ueq));
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

void WillhalmScanner128::compareAndWrite(__m128i shuffled, __m128i mask, __m128i shift, __m128i val1, __m128i val2,
                                         int count) {
    __m128i shifted = _mm_srlv_epi32(shuffled, shift);
    __m128i result;
    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq:
            result = _mm_cmpeq_epi32(shifted, val1);
            break;
        case opr_in: {
            __m128i lower = _mm_cmpgt_epi32(shifted, val1);
            __m128i upper = _mm_cmpgt_epi32(shifted, val2);
            __m128i leq = _mm_cmpeq_epi32(shifted, val1);
            __m128i ueq = _mm_cmpeq_epi32(shifted, val2);
            result = _mm_or_si128(_mm_xor_si128(lower, upper),
                                  _mm_or_si128(leq, ueq));
        }
            break;
        default:
            break;
    }
    writeToDest(result, count);
}

void WillhalmScanner128::scanAligned() {
    __m128i *mdata = (__m128i *) data;

    __m128i val1 = _mm_set1_epi32(p->getVal1());
    __m128i val2 = _mm_set1_epi32(p->getVal2());

    __m128i prev = _mm_set1_epi32(0);
    __m128i current;

    uint64_t laneCounter = 0;
    uint64_t numLane = length * entrySize / SIMD_LEN
                       + (length * entrySize % SIMD_LEN ? 1 : 0);
    int offset = 0;

    while (laneCounter < numLane) {
        current = _mm_stream_load_si128(mdata + laneCounter);
        __m128i mask, shift;

        // Process cross-boundary entry
        if (offset) {
            // Shift prev and current to align
            int inprev = entrySize - offset;
            int inprevByte = inprev / BYTE_LEN + (inprev % BYTE_LEN ? 1 : 0);
            int alignByte = SIMD_LEN / BYTE_LEN - inprevByte;
            __m128i aligned = mm_alignr_epi8(current, prev, alignByte);

            // Read entries from aligned lane
            int alignedOffset = BYTE_LEN - inprev % BYTE_LEN;

            int numEntry = MIN(STEP, ((SIMD_LEN - alignedOffset) / entrySize));
            __m128i shuffled = shuffle(aligned, alignedOffset, entrySize, &shift, &mask);

            compareAndWrite(shuffled, mask, shift, val1, val2, numEntry);
            // Compute the offset in current
            offset += (numEntry - 1) * entrySize;
        }

        while (SIMD_LEN - offset >= entrySize) {
            int numEntry = MIN(STEP, (SIMD_LEN - offset) / entrySize);
            __m128i shuffled = shuffle(current, offset, entrySize, &shift,
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

