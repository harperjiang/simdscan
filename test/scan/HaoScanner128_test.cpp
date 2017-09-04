/*
 * HaoScanner_test.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include "../../src/scan/HaoScanner128.h"
#include <immintrin.h>
#include <gtest/gtest.h>

extern __m128i build128(int num, int bitLength);
extern __m128i buildMask128(int bitLength);

TEST(HaoScanner128, TestBuild128) {
	__m128i b = build128(3, 5);

	ASSERT_EQ(0xc6318c63, _mm256_extract_epi32(b,0));
	ASSERT_EQ(0x318c6318, _mm256_extract_epi32(b,1));
	ASSERT_EQ(0x8c6318c6, _mm256_extract_epi32(b,2));
	ASSERT_EQ(0x6318c631, _mm256_extract_epi32(b,3));

	b = build128(26, 5);

	ASSERT_EQ(0xb5ad6b5a, _mm256_extract_epi32(b,0));
	ASSERT_EQ(0xad6b5ad6, _mm256_extract_epi32(b,1));
	ASSERT_EQ(0x6b5ad6b5, _mm256_extract_epi32(b,2));
	ASSERT_EQ(0x5ad6b5ad, _mm256_extract_epi32(b,3));

}

TEST(HaoScanner128, TestBuildMask128) {
	__m128i b = buildMask128(5);

	ASSERT_EQ(0x21084210, _mm256_extract_epi32(b,0));
	ASSERT_EQ(0x08421084, _mm256_extract_epi32(b,1));
	ASSERT_EQ(0x42108421, _mm256_extract_epi32(b,2));
	ASSERT_EQ(0x10842108, _mm256_extract_epi32(b,3));
}
