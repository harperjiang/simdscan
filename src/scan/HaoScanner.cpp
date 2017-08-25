/*
 * HaoScanner.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include <immintrin.h>
#include "HaoScanner.h"

#define INT_LEN 32
#define SIMD_LEN 256

__m256i build(int num, int bitLength) {
	int offset = 0;
	int r[8];

	for (int i = 0; i < 8; i++) {
		int val = 0;
		int current = offset;
		while (current < INT_LEN) {
			int shift = INT_LEN - bitLength - current;
			if (shift > 0) {
				val |= (num << shift);
			} else if (shift < 0) {
				val |= (num >> -shift);
			} else {
				val |= num;
			}
			current += bitLength;
		}
		offset = bitLength - (INT_LEN % bitLength) + offset;
	}

	return _mm256_setr_epi32(r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
}

__m256i buildMask(int bitLength) {
	return build(1 << (bitLength - 1), bitLength);
}

HaoScanner::HaoScanner(int bs) {
	this->bitSize = bs;
}

HaoScanner::~HaoScanner() {

}

int* HaoScanner::scan(int* data, int length, int* dest, Predicate* p) {
	this->data = data;
	this->dest = dest;
	this->length = length;
	this->val1 = val1;
	this->val2 = val2;

	switch (p->getOpr()) {
	case eq:
	case neq:
		eq();
		break;
	case in:
		in();
		break;
	default:
		break;
	}
	return this->dest;
}

void HaoScanner::eq() {
// Build comparator
	__m256i eqnum = build(this->val1, this->bitSize);
	__m256i mask = buildMask(this->bitSize);
	__m256i one = _mm256_set1_epi32(0xffff);
	__m256i notmask = _mm256_xor_si256(mask, one);

	__m256i current;
	int offset = 0;
	int step = 256 / 32;

	while (offset < length) {
		current = _mm256_stream_load_si256((__m256i *) (data + offset));
		__m256i d = _mm256_xor_si256(current, eqnum);
		__m256i result = _mm256_or_si256(
				_mm256_add_epi32(_mm256_and_si256(d, notmask), notmask), d);
		_mm256_stream_si256((__m256i *) (dest + offset), result);
		offset += step;
	}
}

void HaoScanner::in() {

	__m256i a = build(this->val1, this->bitSize);
	__m256i b = build(this->val2, this->bitSize);

	__m256i one = _mm256_set1_epi32(0xffff);
	__m256i na = _mm256_xor_si256(a, one);
	__m256i nb = _mm256_xor_si256(b, one);

	__m256i mask = buildMask(this->bitSize);
	__m256i notmask = _mm256_xor_si256(mask, one);

	__m256i aornm = _mm256_and_si256(a, notmask);
	__m256i bornm = _mm256_and_si256(b, notmask);

	__m256i current;
	int offset = 0;
	int step = 256 / 32;

	while (offset < length) {
		current = _mm256_stream_load_si256((__m256i *) (data + offset));
		__m256i xorm = _mm256_or_si256(current, mask);
		__m256i l = _mm256_sub_epi32(xorm, aornm);
		__m256i h = _mm256_sub_epi32(xorm, bornm);
		__m256i el = _mm256_and_si256(_mm256_or_si256(current, na),
				_mm256_or_si256(_mm256_and_si256(current, na), l));
		__m256i eh = _mm256_and_si256(_mm256_or_si256(current, nb),
				_mm256_or_si256(_mm256_and_si256(current, nb), h));
		__m256i result = _mm256_xor_si256(el, eh);
		_mm256_stream_si256((__m256i *) (dest + offset), result);
		offset += step;
	}

}

