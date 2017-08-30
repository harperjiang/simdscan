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
#define BYTE_LEN 8

int extract(__m128i data, int index) {
	switch (index) {
	case 0:
		return _mm_extract_epi32(data, 0);
	case 1:
		return _mm_extract_epi32(data, 1);
	case 2:
		return _mm_extract_epi32(data, 2);
	case 3:
	default:
		return _mm_extract_epi32(data, 3);
	}
}

__m128i align(__m128i a, __m128i b, int offset) {
	switch (offset) {
	case 0:
		return b;
	case 1:
		return _mm_alignr_epi8(a, b, 1);
	case 2:
		return _mm_alignr_epi8(a, b, 2);
	case 3:
		return _mm_alignr_epi8(a, b, 3);
	case 4:
		return _mm_alignr_epi8(a, b, 4);
	case 5:
		return _mm_alignr_epi8(a, b, 5);
	case 6:
		return _mm_alignr_epi8(a, b, 6);
	case 7:
		return _mm_alignr_epi8(a, b, 7);
	case 8:
		return _mm_alignr_epi8(a, b, 8);
	case 9:
		return _mm_alignr_epi8(a, b, 9);
	case 10:
		return _mm_alignr_epi8(a, b, 10);
	case 11:
		return _mm_alignr_epi8(a, b, 11);
	case 12:
		return _mm_alignr_epi8(a, b, 12);
	case 13:
		return _mm_alignr_epi8(a, b, 13);
	case 14:
		return _mm_alignr_epi8(a, b, 14);
	case 15:
		return _mm_alignr_epi8(a, b, 15);
	default:
		return b;
	}
}

int computeShuffle(int* shuffleIdx, int* maskIdx, int* shiftIdx, int offset,
		int entrySize) {
	int step = SIMD_LEN / INT_LEN;
	// Each of the 32-bit entry
	for (int j = 0; j < step; j++) {
		int startByte = offset / BYTE_LEN;
		int stopByte = (offset + entrySize) / BYTE_LEN;
		shuffleIdx[j * 4] = startByte;
		shuffleIdx[j * 4 + 1] =
				(stopByte - startByte >= 1) ? startByte + 1 : 0xff;
		shuffleIdx[j * 4 + 2] =
				(stopByte - startByte >= 2) ? startByte + 2 : 0xff;
		shuffleIdx[j * 4 + 3] =
				(stopByte - startByte >= 3) ? startByte + 3 : 0xff;

		int remain = entrySize;
		int deduct = offset % BYTE_LEN;
		maskIdx[j * 4] = 0xff << deduct;
		remain -= (8 - deduct);
		deduct = remain >= BYTE_LEN ? BYTE_LEN : remain;
		maskIdx[j * 4 + 1] = 0xff >> (BYTE_LEN - deduct);
		remain -= deduct;
		deduct = remain >= BYTE_LEN ? BYTE_LEN : remain;
		maskIdx[j * 4 + 2] = 0xff >> (BYTE_LEN - deduct);
		remain -= deduct;
		deduct = remain >= BYTE_LEN ? BYTE_LEN : remain;
		maskIdx[j * 4 + 3] = 0xff >> (BYTE_LEN - deduct);

		shiftIdx[j] = offset % INT_LEN;
	}
	return offset + entrySize * step;
}

WillhalmScanner128::WillhalmScanner128(int entrySize) {
	assert(entrySize < INT_LEN && entrySize > 0);
	this->entrySize = entrySize;
	this->dest = NULL;
	this->destCounter = 0;
}

WillhalmScanner128::~WillhalmScanner128() {
	// TODO Auto-generated destructor stub
}

void WillhalmScanner128::writeToDest(__m128i result, int count) {
	for (int i = 0; i < count; i++)
		*(dest + destCounter) = extract(result, i);
	destCounter += count;
}

void WillhalmScanner128::writeToDest(int result) {
	*(dest + (destCounter++)) = result;
}

void WillhalmScanner128::scan(int* data, int length, int* dest, Predicate* p) {
	this->dest = dest;
	this->destCounter = 0;

	int step = SIMD_LEN / INT_LEN;
	// XXX For experimental purpose, assume data is aligned for now
	assert(length % step == 0);

	__m128i val1 = _mm_set1_epi32(p->getVal1());
	__m128i val2 = _mm_set1_epi32(p->getVal2());

	__m128i prev = _mm_set1_epi32(0);
	__m128i current;

	int entryCounter = 0;
	int entryOffset = 0;

	int wordCounter = 0;

	while (wordCounter < length) {
		current = _mm_stream_load_si128((__m128i *) (data + wordCounter));

		// Process cross-boundary entry
		int remain = wordCounter * BYTE_LEN % entrySize;
		int offset = entrySize - remain;
		if (remain) {
			int remainByte = remain / BYTE_LEN + remain % BYTE_LEN ? 1 : 0;
			int alignByte = SIMD_LEN / BYTE_LEN - remainByte;
			__m128i aligned = align(prev, current, alignByte);

			// Read the cross-boundary entry
			long long data = _mm_extract_epi64(aligned, 0);
			int entryVal = (int) (data >> (remain % BYTE_LEN) & 0xffffffff);
			switch (p->getOpr()) {
			case opr_eq:
				writeToDest(entryVal == p->getVal1() ? 0xffffffff : 0);
				break;
			case opr_neq:
				writeToDest(entryVal != p->getVal1() ? 0xffffffff : 0);
				break;
			case opr_in:
				writeToDest(
						(entryVal <= p->getVal2() && entryVal >= p->getVal1()) ?
								0xffffffff : 0);
				break;
			}
		}

		int numEntry = (SIMD_LEN - offset) / entrySize;

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

			__m128i shuffled = _mm_shuffle_epi8(current, shuffle);
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
			writeToDest(result,
					numEntry - i * step >= 4 ? 4 : numEntry - i * step);
		}

		entryCounter += numEntry;

		wordCounter += step;
		// Store last data in prev
		prev = current;
	}

}

