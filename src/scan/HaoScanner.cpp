/*
 * HaoScanner.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include <immintrin.h>
#include <assert.h>
#include "HaoScanner.h"
#include "../util/math_util.h"

#define INT_LEN 32
#define SIMD_LEN 256
#define BYTE_LEN 8

const __m256i one = _mm256_set1_epi32(0xffffffff);

__m256i build(int num, int bitLength, int offset) {
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

__m256i buildMask(int bitLength, int offset) {
	return build(1 << (bitLength - 1), bitLength, offset);
}

HaoScanner::HaoScanner(int bs) {
	assert(entrySize < 32 && entrySize > 0);
	this->entrySize = bs;

	int ALIGN = SIMD_LEN / BYTE_LEN;
	int LEN = 8 * ALIGN;

	this->val1s = (__m256i *) aligned_alloc(ALIGN, LEN);
	this->val2s = (__m256i *) aligned_alloc(ALIGN, LEN);
	this->nval1s = (__m256i *) aligned_alloc(ALIGN, LEN);
	this->nval2s = (__m256i *) aligned_alloc(ALIGN, LEN);
	this->msbmasks = (__m256i *) aligned_alloc(ALIGN, LEN);
	this->notmasks = (__m256i *) aligned_alloc(ALIGN, LEN);
	this->nmval1s = (__m256i *) aligned_alloc(ALIGN, LEN);
	this->nmval2s = (__m256i *) aligned_alloc(ALIGN, LEN);

	for (int i = 0; i < BYTE_LEN; i++) {
		this->msbmasks[i] = buildMask(entrySize, i);
		this->notmasks[i] = _mm256_xor_si256(this->msbmasks[i], one);
	}
}

HaoScanner::~HaoScanner() {
	free(this->val1s);
	free(this->val2s);
	free(this->nval1s);
	free(this->nval2s);
	free(this->nmval1s);
	free(this->nmval2s);
	free(this->msbmasks);
	free(this->notmasks);
}

void HaoScanner::scan(int* data, int length, int* dest, Predicate* p) {
	// XXX For experimental purpose, assume data is aligned for now
	assert(length % (SIMD_LEN/INT_LEN) == 0);

	this->data = data;
	this->dest = dest;
	this->length = length;

	for (int i = 0; i < BYTE_LEN; i++) {
		this->val1s[i] = build(p->getVal1(), entrySize, i);
		this->val2s[i] = build(p->getVal2(), entrySize, i);
		this->nval1s[i] = _mm256_xor_si256(this->val1s[i], one);
		this->nval2s[i] = _mm256_xor_si256(this->val2s[i], one);
		this->nmval1s[i] = _mm256_and_si256(this->val1s[i], this->notmasks[i]);
		this->nmval2s[i] = _mm256_and_si256(this->val2s[i], this->notmasks[i]);
	}

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
	void* byteData = data;
	void* byteDest = dest;
	int byteOffset = 0;
	int bitOffset = 0;

	int entryCounter = 0;

	while (entryCounter < length) {
		__m256i eqnum = this->val1s[bitOffset];
		__m256i notmask = this->notmasks[bitOffset];

		__m256i current = _mm256_loadu_si256(
				(__m256i *) (byteData + byteOffset));
		__m256i d = _mm256_xor_si256(current, eqnum);
		__m256i result = _mm256_or_si256(
				mm256_add_epi256(_mm256_and_si256(d, notmask), notmask), d);
		__m256i resmask = _mm256_setr_epi64x(-1 << bitOffset, -1, -1, -1);
		int existMask = (1 << bitOffset) - 1;
		int exist = (int) *((char*) (byteDest + byteOffset));

		__m256i joined = _mm256_xor_si256(_mm256_and_si256(resmask, result),
				_mm256_setr_epi64x(existMask & exist, 0, 0, 0));

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

	void* byteData = data;
	void* byteDest = dest;
	int byteOffset = 0;
	int bitOffset = 0;

	int entryCounter = 0;

	while (entryCounter < length) {
		__m256i mask = this->msbmasks[bitOffset];
		__m256i aornm = this->nmval1s[bitOffset];
		__m256i bornm = this->nmval2s[bitOffset];
		__m256i na = this->nval1s[bitOffset];
		__m256i nb = this->nval2s[bitOffset];

		__m256i current = _mm256_loadu_si256(
				(__m256i *) (byteData + byteOffset));
		__m256i xorm = _mm256_or_si256(current, mask);
		__m256i l = mm256_sub_epi256(xorm, aornm);
		__m256i h = mm256_sub_epi256(xorm, bornm);
		__m256i el = _mm256_and_si256(_mm256_or_si256(current, na),
				_mm256_or_si256(_mm256_and_si256(current, na), l));
		__m256i eh = _mm256_and_si256(_mm256_or_si256(current, nb),
				_mm256_or_si256(_mm256_and_si256(current, nb), h));
		__m256i result = _mm256_xor_si256(el, eh);

		__m256i resmask = _mm256_setr_epi64x(-1 << bitOffset, -1, -1, -1);
		int existMask = (1 << bitOffset) - 1;
		int exist = (int) *((char*) (byteDest + byteOffset));

		__m256i joined = _mm256_xor_si256(_mm256_and_si256(resmask, result),
				_mm256_setr_epi64x(existMask & exist, 0, 0, 0));

		_mm256_storeu_si256((__m256i *) (byteDest + byteOffset), joined);

		int numFullEntry = (SIMD_LEN - bitOffset) / entrySize;
		entryCounter += numFullEntry;
		int bitAdvance = (bitOffset + numFullEntry * entrySize);
		int byteAdvance = bitAdvance / BYTE_LEN;
		byteOffset += byteAdvance;
		bitOffset = bitAdvance % BYTE_LEN;
	}

}

