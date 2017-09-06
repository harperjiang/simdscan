/*
 * math_test.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: harper
 */

#include <immintrin.h>
#include <gtest/gtest.h>
#include "../../src/util/math_util.h"

TEST(Math, TestAddEpi128) {
	__m128i a = _mm_setr_epi32(0, 0x80000000, 0, 0);
	__m128i b = _mm_setr_epi32(0, 0x80000000, 0, 0);

	__m128i result = mm_add_epi128(a, b);

	EXPECT_EQ(0, _mm_extract_epi32(result,0));
	EXPECT_EQ(0, _mm_extract_epi32(result,1));
	EXPECT_EQ(1, _mm_extract_epi32(result,2));
	EXPECT_EQ(0, _mm_extract_epi32(result,3));

	__m128i result2 = mm_add_epi128_2(a, b);

	EXPECT_EQ(0, _mm_extract_epi32(result2,0));
	EXPECT_EQ(0, _mm_extract_epi32(result2,1));
	EXPECT_EQ(1, _mm_extract_epi32(result2,2));
	EXPECT_EQ(0, _mm_extract_epi32(result2,3));

	__m128i result3 = mm_add_epi128_3(a, b);

	EXPECT_EQ(0, _mm_extract_epi32(result3,0));
	EXPECT_EQ(0, _mm_extract_epi32(result3,1));
	EXPECT_EQ(1, _mm_extract_epi32(result3,2));
	EXPECT_EQ(0, _mm_extract_epi32(result3,3));
}

TEST(Math, TestSubEpi128) {

	__m128i a = _mm_setr_epi32(0, 0, 1, 0);
	__m128i b = _mm_setr_epi32(0, 0x80000000, 0, 0);

	__m128i result = mm_sub_epi128(a, b);

	EXPECT_EQ(0, _mm_extract_epi32(result,0));
	EXPECT_EQ((int )0x80000000, _mm_extract_epi32(result,1));
	EXPECT_EQ(0, _mm_extract_epi32(result,2));
	EXPECT_EQ(0, _mm_extract_epi32(result,3));
}
