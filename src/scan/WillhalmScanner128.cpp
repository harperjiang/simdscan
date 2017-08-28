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

__m128i SHUFFLE_1[] = { };
__m128i SHUFFLE_2[] = { };
__m128i SHUFFLE_3[] = { };
__m128i SHUFFLE_4[] = { };
__m128i SHUFFLE_5[] = { };
__m128i SHUFFLE_6[] = { };
__m128i SHUFFLE_7[] = { };
__m128i SHUFFLE_8[] = { };
__m128i SHUFFLE_9[] = { _mm_setr_epi8(0, 1, 0xff, 0xff, 1, 2, 0xff, 0xff, 2, 3,
		0xff, 0xff, 3, 4, 0xff, 0xff), _mm_setr_epi8(4, 5, 0xff, 0xff, 5, 6,
		0xff, 0xff, 6, 7, 0xff, 0xff, 7, 8, 0xff, 0xff), _mm_setr_epi8(9, 0xa,
		0xff, 0xff, 0xa, 0xb, 0xff, 0xff, 0xb, 0xc, 0xff, 0xff, 0xc, 0xd, 0xff,
		0xff), _mm_setr_epi8(0xd, 0xe, 0xff, 0xff, 0xe, 0xf, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff) };
__m128i SHUFFLE_10[] = { };
__m128i SHUFFLE_11[] = { };
__m128i SHUFFLE_12[] = { };
__m128i SHUFFLE_13[] = { };
__m128i SHUFFLE_14[] = { };
__m128i SHUFFLE_15[] = { };
__m128i SHUFFLE_16[] = { };
__m128i SHUFFLE_17[] = { };
__m128i SHUFFLE_18[] = { };
__m128i SHUFFLE_19[] = { };
__m128i SHUFFLE_20[] = { };
__m128i SHUFFLE_21[] = { };
__m128i SHUFFLE_22[] = { };
__m128i SHUFFLE_23[] = { };
__m128i SHUFFLE_24[] = { };
__m128i SHUFFLE_25[] = { };
__m128i SHUFFLE_26[] = { };
__m128i SHUFFLE_27[] = { };
__m128i SHUFFLE_28[] = { };
__m128i SHUFFLE_29[] = { };
__m128i SHUFFLE_30[] = { };
__m128i SHUFFLE_31[] = { };
__m128i* SHUFFLES[] = {SHUFFLE_1, SHUFFLE_2, SHUFFLE_3, SHUFFLE_4, SHUFFLE_5,
	SHUFFLE_6, SHUFFLE_7, SHUFFLE_8, SHUFFLE_9, SHUFFLE_10, SHUFFLE_11,
	SHUFFLE_12, SHUFFLE_13, SHUFFLE_14, SHUFFLE_15, SHUFFLE_16, SHUFFLE_17,
	SHUFFLE_18, SHUFFLE_19, SHUFFLE_20, SHUFFLE_21, SHUFFLE_22, SHUFFLE_23,
	SHUFFLE_24, SHUFFLE_25, SHUFFLE_26, SHUFFLE_27, SHUFFLE_28, SHUFFLE_29,
	SHUFFLE_30, SHUFFLE_31};

__m128i MASK_1[] = { };
__m128i MASK_2[] = { };
__m128i MASK_3[] = { };
__m128i MASK_4[] = { };
__m128i MASK_5[] = { };
__m128i MASK_6[] = { };
__m128i MASK_7[] = { };
__m128i MASK_8[] = { };
__m128i MASK_9[] = { _mm_setr_epi32(0x1ff, 0x3fe, 0x7fc, 0xff8), _mm_setr_epi32(
		0x1ff0, 0x3fe0, 0x7fc0, 0xff80), _mm_setr_epi32(0x1ff, 0x3fe, 0x7fc,
		0xff8), _mm_setr_epi32(0x1ff0, 0x3fe0, 0x0, 0x0) };
__m128i MASK_10[] = { };
__m128i MASK_11[] = { };
__m128i MASK_12[] = { };
__m128i MASK_13[] = { };
__m128i MASK_14[] = { };
__m128i MASK_15[] = { };
__m128i MASK_16[] = { };
__m128i MASK_17[] = { };
__m128i MASK_18[] = { };
__m128i MASK_19[] = { };
__m128i MASK_20[] = { };
__m128i MASK_21[] = { };
__m128i MASK_22[] = { };
__m128i MASK_23[] = { };
__m128i MASK_24[] = { };
__m128i MASK_25[] = { };
__m128i MASK_26[] = { };
__m128i MASK_27[] = { };
__m128i MASK_28[] = { };
__m128i MASK_29[] = { };
__m128i MASK_30[] = { };
__m128i MASK_31[] = { };
__m128i *MASKS[] = { MASK_1, MASK_2, MASK_3, MASK_4, MASK_5, MASK_6, MASK_7,
		MASK_8, MASK_9, MASK_10, MASK_11, MASK_12, MASK_13, MASK_14, MASK_15,
		MASK_16, MASK_17, MASK_18, MASK_19, MASK_20, MASK_21, MASK_22, MASK_23,
		MASK_24, MASK_25, MASK_26, MASK_27, MASK_28, MASK_29, MASK_30, MASK_31 };

__m128i * SHIFTS[] = { };

__m128i mm_alignr_epi8(__m128i a, __m128i b, int shift) {
	switch (shift) {
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
	default:
		return b;
	}

}

int WillhalmScanner128::computeShift(int counter) {
	return (counter * SIMD_LEN) % this->bitSize;
}

void WillhalmScanner128::computePredicate(Predicate* p) {
	__m128i* shifts = SHIFTS[this->bitSize - 1];

	switch (p->getOpr()) {
		case opr_eq:
		case opr_neq: {
			__m128i num = _mm_set1_epi32(p->getVal1());
			this->val1 = new __m128i[this->shuffleCount];
			for (int i = 0; i < this->shuffleCount; i++) {
				this->val1[i] = _mm_sll_epi32(num, shifts[i]);
			}
		}
		break;
		case opr_in: {
			__m128i numl = _mm_set1_epi32(p->getVal1());
			__m128i numu = _mm_set1_epi32(p->getVal2());
			this->val1 = new __m128i[this->shuffleCount];
			this->val2 = new __m128i[this->shuffleCount];
			for (int i = 0; i < this->shuffleCount; i++) {
				this->val1[i] = _mm_sll_epi32(numl, shifts[i]);
				this->val2[i] = _mm_sll_epi32(numu, shifts[i]);
			}
		}
		break;
		default:
		break;
	}
}

WillhalmScanner128::WillhalmScanner128(int bitSize) {
	assert(bitSize < 32 && bitSize > 0);
	this->bitSize = bitSize;

	// Compute shuffle related parameters
	int numEntries = SIMD_LEN / bitSize;
	int step = SIMD_LEN / INT_LEN;
	this->shuffleCount = numEntries / step + (numEntries % step) ? 1 : 0;

	this->shuffles = SHUFFLES[bitSize - 1];
	this->compareMask = MASKS[bitSize - 1];

	this->val1 = NULL;
	this->val2 = NULL;
}

WillhalmScanner128::~WillhalmScanner128() {
	// TODO Auto-generated destructor stub
}

void WillhalmScanner128::scan(int* data, int length, int* dest, Predicate* p) {
	int readCounter = 0;
	int writeCounter = 0;
	int step = SIMD_LEN / INT_LEN;
	// XXX For experimental purpose, assume data is aligned for now
	assert(length % step == 0);

	//
	computePredicate(p);

	__m128i prev = _mm_set1_epi32(0);
	__m128i current;
	while (readCounter < length) {
		current = _mm_stream_load_si128((__m128i *) (data + readCounter));
		// Shift data to have 16-bytes alignment
		int shift = computeShift(readCounter);
		if (shift != 0) {
			current = mm_alignr_epi8(prev, current, shift);
		}

		for (int i = 0; i < shuffleCount; i++) {
			// Use shuffle to make 4-byte alignment
			__m128i shuffled = _mm_shuffle_epi8(current, shuffles[i]);
			// Compare the aligned data with predicate
			__m128i masked = _mm_and_si128(shuffled, this->compareMask[i]);
			__m128i result;
			switch (p->getOpr()) {
			case opr_eq:
			case opr_neq:
				result = _mm_cmpeq_epi32(masked, val1[i]);
				break;
			case opr_in: {
				__m128i lower = _mm_cmpgt_epi32(masked, val1[i]);
				__m128i upper = _mm_cmpgt_epi32(masked, val2[i]);
				__m128i leq = _mm_cmpeq_epi32(masked, val1[i]);
				__m128i ueq = _mm_cmpeq_epi32(masked, val2[i]);
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
	}

	readCounter += step;
	// Store last data in prev
	prev = current;
}

