/*
 * HaoScanner_test.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include "src/scan/HaoScanner.h"
#include <immintrin.h>
#include <gtest/gtest.h>

extern __m256i build(int num, int bitLength);
extern __m256i buildMask(int bitLength);

TEST(HaoScanner, TestBuild) {

	__m256i b = build(3, 5);

	ASSERT_EQ(0, _mm256_extract_epi32(b,0));
	ASSERT_EQ(0, _mm256_extract_epi32(b,1));
	ASSERT_EQ(0, _mm256_extract_epi32(b,2));
	ASSERT_EQ(0, _mm256_extract_epi32(b,3));
	ASSERT_EQ(0, _mm256_extract_epi32(b,4));
	ASSERT_EQ(0, _mm256_extract_epi32(b,5));
	ASSERT_EQ(0, _mm256_extract_epi32(b,6));
	ASSERT_EQ(0, _mm256_extract_epi32(b,7));
}

TEST(HaoScanner, TestBuildMask) {

	__m256i b = buildMask(5);

	ASSERT_EQ(0, _mm256_extract_epi32(b,0));
	ASSERT_EQ(0, _mm256_extract_epi32(b,1));
	ASSERT_EQ(0, _mm256_extract_epi32(b,2));
	ASSERT_EQ(0, _mm256_extract_epi32(b,3));
	ASSERT_EQ(0, _mm256_extract_epi32(b,4));
	ASSERT_EQ(0, _mm256_extract_epi32(b,5));
	ASSERT_EQ(0, _mm256_extract_epi32(b,6));
	ASSERT_EQ(0, _mm256_extract_epi32(b,7));
}
