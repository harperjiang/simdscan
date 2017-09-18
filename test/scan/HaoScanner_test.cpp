/*
 * HaoScanner_test.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include "../../src/scan/HaoScanner.h"
#include <immintrin.h>
#include <gtest/gtest.h>

extern __m256i build(int num, int bitLength, int);
extern __m256i buildMask(int bitLength, int);

TEST(HaoScanner, TestBuild) {
	__m256i b = build(3, 5,0);

	ASSERT_EQ(0xc6318c63, _mm256_extract_epi32(b,0));
	ASSERT_EQ(0x318c6318, _mm256_extract_epi32(b,1));
	ASSERT_EQ(0x8c6318c6, _mm256_extract_epi32(b,2));
	ASSERT_EQ(0x6318c631, _mm256_extract_epi32(b,3));
	ASSERT_EQ(0x18c6318c, _mm256_extract_epi32(b,4));
	ASSERT_EQ(0xc6318c63, _mm256_extract_epi32(b,5));
	ASSERT_EQ(0x318c6318, _mm256_extract_epi32(b,6));
	ASSERT_EQ(0x8c6318c6, _mm256_extract_epi32(b,7));

	b = build(26, 5,0);

	ASSERT_EQ(0xb5ad6b5a, _mm256_extract_epi32(b,0));
	ASSERT_EQ(0xad6b5ad6, _mm256_extract_epi32(b,1));
	ASSERT_EQ(0x6b5ad6b5, _mm256_extract_epi32(b,2));
	ASSERT_EQ(0x5ad6b5ad, _mm256_extract_epi32(b,3));
	ASSERT_EQ(0xd6b5ad6b, _mm256_extract_epi32(b,4));
	ASSERT_EQ(0xb5ad6b5a, _mm256_extract_epi32(b,5));
	ASSERT_EQ(0xad6b5ad6, _mm256_extract_epi32(b,6));
	ASSERT_EQ(0x6b5ad6b5, _mm256_extract_epi32(b,7));
}

TEST(HaoScanner, TestBuildMask) {
	__m256i b = buildMask(5,0);

	ASSERT_EQ(0x21084210, _mm256_extract_epi32(b,0));
	ASSERT_EQ(0x08421084, _mm256_extract_epi32(b,1));
	ASSERT_EQ(0x42108421, _mm256_extract_epi32(b,2));
	ASSERT_EQ(0x10842108, _mm256_extract_epi32(b,3));
	ASSERT_EQ(0x84210842, _mm256_extract_epi32(b,4));
	ASSERT_EQ(0x21084210, _mm256_extract_epi32(b,5));
	ASSERT_EQ(0x08421084, _mm256_extract_epi32(b,6));
	ASSERT_EQ(0x42108421, _mm256_extract_epi32(b,7));
}
