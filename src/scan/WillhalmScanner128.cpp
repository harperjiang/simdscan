/*
 * WillhalmScanner.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include <immintrin.h>
#include "WillhalmScanner128.h"
#include <assert.h>

#define INT_LEN 32
#define SIMD_LEN 128

int computeShuffle(int* shuffleIdx, int* maskIdx, int* shiftIdx, int offset,
		int entrySize) {
	int step = SIMD_LEN / INT_LEN;
	// Each of the 32-bit entry
	for (int j = 0; j < step; j++) {
		int startByte = offset / 8;
		int stopByte = (offset + entrySize) / 8;
		shuffleIdx[j * 4] = startByte;
		shuffleIdx[j * 4 + 1] =
				(stopByte - startByte >= 1) ? startByte + 1 : 0xff;
		shuffleIdx[j * 4 + 2] =
				(stopByte - startByte >= 2) ? startByte + 2 : 0xff;
		shuffleIdx[j * 4 + 3] =
				(stopByte - startByte >= 3) ? startByte + 3 : 0xff;

		int remain = entrySize;
		int deduct = offset % 8;
		maskIdx[j * 4] = 0xff << deduct;
		remain -= (8 - deduct);
		deduct = remain >= 8 ? 8 : remain;
		maskIdx[j * 4 + 1] = 0xff >> (8 - deduct);
		remain -= deduct;
		deduct = remain >= 8 ? 8 : remain;
		maskIdx[j * 4 + 2] = 0xff >> (8 - deduct);
		remain -= deduct;
		deduct = remain >= 8 ? 8 : remain;
		maskIdx[j * 4 + 3] = 0xff >> (8 - deduct);

		shiftIdx[j] = offset % INT_LEN;
	}
	return offset + entrySize * step;
}

WillhalmScanner128::WillhalmScanner128(int entrySize) {
	assert(entrySize < 32 && entrySize > 0);
	this->entrySize = entrySize;
}

WillhalmScanner128::~WillhalmScanner128() {
	// TODO Auto-generated destructor stub
}

void WillhalmScanner128::scan(int* data, int length, int* dest, Predicate* p) {
	int step = SIMD_LEN / INT_LEN;
	// XXX For experimental purpose, assume data is aligned for now
	assert(length % step == 0);

	__m128i val1 = _mm_set1_epi32(p->getVal1());
	__m128i val2 = _mm_set1_epi32(p->getVal2());

	__m128i prev = _mm_set1_epi32(0);
	__m128i current;

	int entryCounter = 0;
	int entryResidue = 0;

	int readCounter = 0;
	int writeCounter = 0;

	while (readCounter < length) {
		current = _mm_stream_load_si128((__m128i *) (data + readCounter));
		__m128i process = current;
		int numEntry = SIMD_LEN / entrySize;
		int offset = 0;
		// Shift data to have 16-bytes alignment
		if (entryResidue != 0) {
			process = _mm_alignr_epi8(prev, current, 15);
			offset = 8 - entryResidue;
			numEntry = (SIMD_LEN - offset) / entrySize;
		}
		int numGroup = numEntry / step + (numEntry % step) ? 1 : 0;

		for (int i = 0; i < numGroup; i++) {
			int shuffleIdx[16];
			int maskIdx[16];
			int shiftIdx[4];
			offset = computeShuffle(shuffleIdx, maskIdx, shiftIdx, offset,
					entrySize);

			// Use shuffle to make 4-byte alignment
			__m128i shuffle = _mm_setr_epi8(shuffleIdx[0], shuffleIdx[1],
					shuffleIdx[2], shuffleIdx[3], shuffleIdx[4], shuffleIdx[5],
					shuffleIdx[6], shuffleIdx[7], shuffleIdx[8], shuffleIdx[9],
					shuffleIdx[10], shuffleIdx[11], shuffleIdx[12],
					shuffleIdx[13], shuffleIdx[14], shuffleIdx[15]);
			__m128i mask = _mm_setr_epi8(maskIdx[0], maskIdx[1], maskIdx[2],
					maskIdx[3], maskIdx[4], maskIdx[5], maskIdx[6], maskIdx[7],
					maskIdx[8], maskIdx[9], maskIdx[10], maskIdx[11],
					maskIdx[12], maskIdx[13], maskIdx[14], maskIdx[15]);
			__m128i shift = _mm_setr_epi32(shiftIdx[0], shiftIdx[1],
					shiftIdx[2], shiftIdx[3]);

			__m128i shuffled = _mm_shuffle_epi8(process, shuffle);
			// Compare the aligned data with predicate
			__m128i masked = _mm_and_si128(shuffled, mask);
			__m128i result;
			switch (p->getOpr()) {
			case opr_eq:
			case opr_neq:
				result = _mm_cmpeq_epi32(masked, _mm_sllv_epi32(val1, shift));
				break;
			case opr_in: {
				__m128i v1shift = _mm_sllv_epi32(val1, shift);
				__m128i v2shift = _mm_sllv_epi32(val2, shift);
				__m128i lower = _mm_cmpgt_epi32(masked, v1shift);
				__m128i upper = _mm_cmpgt_epi32(masked, v2shift);
				__m128i leq = _mm_cmpeq_epi32(masked, v1shift);
				__m128i ueq = _mm_cmpeq_epi32(masked, v2shift);
				result = _mm_or_si128(_mm_xor_si128(lower, upper),
						_mm_or_si128(leq, ueq));
			}
				break;
			default:
				break;
			}
			_mm_stream_si128((__m128i *) (dest + writeCounter), result);
			writeCounter += step;
		}

		entryCounter += numEntry;
		entryResidue = (SIMD_LEN - entryResidue + 8) % entrySize;
	}

	readCounter += step;
	// Store last data in prev
	prev = current;
}

