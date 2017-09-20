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

int WillhalmScanner256::computeShift(int counter) {
	return 0;
}

WillhalmScanner256::WillhalmScanner256(int bitSize) {
	this->bitSize = bitSize;
}

WillhalmScanner256::~WillhalmScanner256() {
	// TODO Auto-generated destructor stub
}

void WillhalmScanner256::scan(int* data, int length, int* dest, Predicate* p) {
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

