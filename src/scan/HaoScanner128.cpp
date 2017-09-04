/*
 * HaoScanner128.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include <immintrin.h>
#include <assert.h>
#include "HaoScanner128.h"

#define INT_LEN 32
#define SIMD_LEN 128
#define BYTE_LEN 8

__m128i build128(int num, int bitLength) {
	int offset = 0;
	int r[SIMD_LEN / INT_LEN];

	for (int i = 0; i < SIMD_LEN / INT_LEN; i++) {
		int val = 0;
		int current = offset;
		if (offset != 0) {
			val |= (num >> (bitLength - offset));
		}
		while (current < INT_LEN) {
			val |= (num << current);
			current += bitLength;
		}
		r[i] = val;
		offset = bitLength - (INT_LEN - offset) % bitLength;
	}
	return _mm_setr_epi32(r[0], r[1], r[2], r[3]);
}

__m128i buildMask128(int bitLength) {
	return build128(1 << (bitLength - 1), bitLength);
}

HaoScanner128::HaoScanner128(int bs) {
	assert(entrySize < 32 && entrySize > 0);
	this->entrySize = bs;
}

HaoScanner128::~HaoScanner128() {

}

void HaoScanner128::scan(int* data, int length, int* dest, Predicate* p) {
	// XXX For experimental purpose, assume data is aligned for now
	assert(length % (SIMD_LEN/INT_LEN) == 0);

	this->data = data;
	this->dest = dest;
	this->length = length;
	this->val1 = val1;
	this->val2 = val2;

	switch (p->getOpr()) {
	case opr_eq:
	case opr_neq:
		eq();
		break;
	case opr_in:
		in();
		break;
	default:
		break;
	}
}

void HaoScanner128::eq() {
	// Build comparator
	__m128i eqnum = build128(this->val1, this->entrySize);
	__m128i mask = buildMask128(this->entrySize);
	__m128i one = _mm_set1_epi32(0xffff);
	__m128i notmask = _mm_xor_si128(mask, one);

	__m128i current;

	void* byteData = data;
	void* byteDest = dest;
	int byteOffset = 0;
	int bitOffset = 0;

	int entryCounter = 0;

	while (entryCounter < length) {
		current = _mm_loadu_si128((__m128i *) (byteData + byteOffset));
		__m128i d = _mm_xor_si128(current, eqnum);
		__m128i result = _mm_or_si128(
				_mm_add_epi32(_mm_and_si128(d, notmask), notmask), d);
		__m128i mask = _mm_setr_epi32(-1 << bitOffset, -1, -1, -1);
		__m128i exist = _mm_setr_epi32((int)*((char*)(byteData+byteOffset)), 0, 0, 0);

		__m128i joined = _mm_xor_si128(_mm_and_si128(mask, result), exist);

		_mm_storeu_si128((__m128i *) (byteDest + byteOffset), joined);

		int numFullEntry = (SIMD_LEN - bitOffset) / entrySize;
		entryCounter += numFullEntry;
		int bitAdvance = (bitOffset + numFullEntry * entrySize);
		int byteAdvance = bitAdvance / BYTE_LEN;
		byteOffset += byteAdvance;
		bitOffset = bitAdvance % BYTE_LEN;
	}
}

void HaoScanner128::in() {

	__m128i a = build128(this->val1, this->entrySize);
	__m128i b = build128(this->val2, this->entrySize);

	__m128i one = _mm_set1_epi32(0xffff);
	__m128i na = _mm_xor_si128(a, one);
	__m128i nb = _mm_xor_si128(b, one);

	__m128i mask = buildMask128(this->entrySize);
	__m128i notmask = _mm_xor_si128(mask, one);

	__m128i aornm = _mm_and_si128(a, notmask);
	__m128i bornm = _mm_and_si128(b, notmask);

	__m128i current;

	void* byteData = data;
	void* byteDest = dest;
	int byteOffset = 0;
	int bitOffset = 0;

	int entryCounter = 0;

	while (entryCounter < length) {
		current = _mm_loadu_si128((__m128i *) (byteData + byteOffset));
		__m128i xorm = _mm_or_si128(current, mask);
		__m128i l = _mm_sub_epi32(xorm, aornm);
		__m128i h = _mm_sub_epi32(xorm, bornm);
		__m128i el = _mm_and_si128(_mm_or_si128(current, na),
				_mm_or_si128(_mm_and_si128(current, na), l));
		__m128i eh = _mm_and_si128(_mm_or_si128(current, nb),
				_mm_or_si128(_mm_and_si128(current, nb), h));
		__m128i result = _mm_xor_si128(el, eh);

		__m128i mask = _mm_setr_epi32(-1 << bitOffset, -1, -1, -1);
		__m128i exist = _mm_setr_epi32((int) *((char*)(byteData+byteOffset)), 0, 0, 0);

		__m128i joined = _mm_xor_si128(_mm_and_si128(mask, result),
				exist);

		_mm_storeu_si128((__m128i *) (byteDest + byteOffset), joined);

		int numFullEntry = (SIMD_LEN - bitOffset) / entrySize;
		entryCounter += numFullEntry;
		int bitAdvance = (bitOffset + numFullEntry * entrySize);
		int byteAdvance = bitAdvance / BYTE_LEN;
		byteOffset += byteAdvance;
		bitOffset = bitAdvance % BYTE_LEN;
	}

}

