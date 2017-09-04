/*
 * HaoScanner.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include <immintrin.h>
#include <assert.h>
#include "HaoScanner.h"

#define INT_LEN 32
#define SIMD_LEN 256
#define BYTE_LEN 8

__m256i build(int num, int bitLength) {
	int offset = 0;
	int r[8];

	for (int i = 0; i < 8; i++) {
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
	return _mm256_setr_epi32(r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
}

__m256i buildMask(int bitLength) {
	return build(1 << (bitLength - 1), bitLength);
}

HaoScanner::HaoScanner(int bs) {
	assert(entrySize < 32 && entrySize > 0);
	this->entrySize = bs;
}

HaoScanner::~HaoScanner() {

}

void HaoScanner::scan(int* data, int length, int* dest, Predicate* p) {
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

void HaoScanner::eq() {
	// Build comparator
	__m256i eqnum = build(this->val1, this->entrySize);
	__m256i mask = buildMask(this->entrySize);
	__m256i one = _mm256_set1_epi32(0xffff);
	__m256i notmask = _mm256_xor_si256(mask, one);

	__m256i current;

	void* byteData = data;
	void* byteDest = dest;
	int byteOffset = 0;
	int bitOffset = 0;

	int entryCounter = 0;

	while (entryCounter < length) {
		current = _mm256_loadu_si256((__m256i *) (byteData + byteOffset));
		__m256i d = _mm256_xor_si256(current, eqnum);
		__m256i result = _mm256_or_si256(
				_mm256_add_epi32(_mm256_and_si256(d, notmask), notmask), d);
		__m256i mask = _mm256_setr_epi32(-1 << bitOffset, -1, -1, -1, -1, -1,
				-1, -1);
		__m256i exist = _mm256_setr_epi32((int) *((char*)(byteData+byteOffset)), 0, 0, 0,
				0, 0, 0, 0);

		__m256i joined = _mm256_xor_si256(_mm256_and_si256(mask, result),
				exist);

		_mm256_storeu_si256((__m256i *) (byteDest + byteOffset), joined);

		int numFullEntry = (SIMD_LEN - bitOffset) / entrySize;
		entryCounter += numFullEntry;
		int bitAdvance = (bitOffset + numFullEntry * entrySize);
		int byteAdvance = bitAdvance / BYTE_LEN;
		byteOffset += byteAdvance;
		bitOffset = bitAdvance % BYTE_LEN;
	}
}

void HaoScanner::in() {

	__m256i a = build(this->val1, this->entrySize);
	__m256i b = build(this->val2, this->entrySize);

	__m256i one = _mm256_set1_epi32(0xffff);
	__m256i na = _mm256_xor_si256(a, one);
	__m256i nb = _mm256_xor_si256(b, one);

	__m256i mask = buildMask(this->entrySize);
	__m256i notmask = _mm256_xor_si256(mask, one);

	__m256i aornm = _mm256_and_si256(a, notmask);
	__m256i bornm = _mm256_and_si256(b, notmask);

	__m256i current;

	void* byteData = data;
	void* byteDest = dest;
	int byteOffset = 0;
	int bitOffset = 0;

	int entryCounter = 0;

	while (entryCounter < length) {
		current = _mm256_loadu_si256((__m256i *) (byteData + byteOffset));
		__m256i xorm = _mm256_or_si256(current, mask);
		__m256i l = _mm256_sub_epi32(xorm, aornm);
		__m256i h = _mm256_sub_epi32(xorm, bornm);
		__m256i el = _mm256_and_si256(_mm256_or_si256(current, na),
				_mm256_or_si256(_mm256_and_si256(current, na), l));
		__m256i eh = _mm256_and_si256(_mm256_or_si256(current, nb),
				_mm256_or_si256(_mm256_and_si256(current, nb), h));
		__m256i result = _mm256_xor_si256(el, eh);

		__m256i mask = _mm256_setr_epi32(-1 << bitOffset, -1, -1, -1, -1, -1,
				-1, -1);
		__m256i exist = _mm256_setr_epi32((int) *((char*)(byteData+byteOffset)), 0, 0, 0,
				0, 0, 0, 0);

		__m256i joined = _mm256_xor_si256(_mm256_and_si256(mask, result),
				exist);

		_mm256_storeu_si256((__m256i *) (byteDest + byteOffset), joined);

		int numFullEntry = (SIMD_LEN - bitOffset) / entrySize;
		entryCounter += numFullEntry;
		int bitAdvance = (bitOffset + numFullEntry * entrySize);
		int byteAdvance = bitAdvance / BYTE_LEN;
		byteOffset += byteAdvance;
		bitOffset = bitAdvance % BYTE_LEN;
	}

}

