/*
 * math_test.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: harper
 */

#include <immintrin.h>
#include <gtest/gtest.h>

extern __m128i mm_add_epi128(__m128i, __m128i);
extern __m128i mm_sub_epi128(__m128i, __m128i);

TEST(Math, TestAddEpi128) {

	__m128i a = _mm_setr_epi32(0,0x80000000,0,0);
	__m128i b = _mm_setr_epi32(0,0x80000000,0,0);

	__m128i result = mm_add_epi128(a,b);

	EXPECT_EQ(0, _mm_extract_epi32(result,0));
	EXPECT_EQ(0, _mm_extract_epi32(result,1));
	EXPECT_EQ(1, _mm_extract_epi32(result,2));
	EXPECT_EQ(0, _mm_extract_epi32(result,3));
}

TEST(Math, TestSubEpi128) {

	__m128i a = _mm_setr_epi32(0,0,1,0);
	__m128i b = _mm_setr_epi32(0,0x80000000,0,0);

	__m128i result = mm_sub_epi128(a,b);

	EXPECT_EQ(0, _mm_extract_epi32(result,0));
	EXPECT_EQ((int)0x80000000, _mm_extract_epi32(result,1));
	EXPECT_EQ(0, _mm_extract_epi32(result,2));
	EXPECT_EQ(0, _mm_extract_epi32(result,3));
}
