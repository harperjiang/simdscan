/*
 * math_test.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: harper
 */

#include <stdio.h>
#include <immintrin.h>
#include <gtest/gtest.h>
#include <sys/time.h>
#include "../../src/util/math_util.h"

TEST(Math, TestAddEpi128Perf) {

	int repeat = 10000000;
	long start, elapse;

	__m128i* a0 = new __m128i[repeat];
	__m128i* b0 = new __m128i[repeat];

	__m128i* a1 = new __m128i[repeat];
	__m128i* b1 = new __m128i[repeat];

	__m128i* a2 = new __m128i[repeat];
	__m128i* b2 = new __m128i[repeat];

	for(int i = 0 ; i < repeat ; i++) {
		a0[i] =  _mm_setr_epi32(0, 3*i,0,i);
		b0[i] =  _mm_setr_epi32(i,0,0,2*i);

		a1[i] = _mm_setr_epi32(0,i,i,i);
		b1[i] = _mm_setr_epi32(2*i,0,2*i,2*i);

		a2[i] = _mm_setr_epi32(0,0x80000000+i,0, i);
		b2[i] = _mm_setr_epi32(0,0x80000000+2*i,0, 2*i);
	}

	struct timeval tp;
	gettimeofday(&tp, NULL);
	start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

	for(int i = 0 ; i < repeat;i++) {
		__m128i r0 = mm_add_epi128_2(a0[i],b0[i]);

		EXPECT_EQ(i, _mm_extract_epi32(r0,0)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r0,1)) << i;
		EXPECT_EQ(0, _mm_extract_epi32(r0,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r0,3)) << i;

		__m128i r1 = mm_add_epi128_2(a1[i],b1[i]);

		EXPECT_EQ(2*i, _mm_extract_epi32(r1,0)) << i;
		EXPECT_EQ(i, _mm_extract_epi32(r1,1)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r1,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r1,3)) << i;

		__m128i r2 = mm_add_epi128_2(a2[i],b2[i]);

		EXPECT_EQ(0, _mm_extract_epi32(r2,0)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r2,1)) << i;
		EXPECT_EQ(1, _mm_extract_epi32(r2,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r2,3)) << i;

	}
	gettimeofday(&tp, NULL);
	elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;
	std::cout << "perf of add 2:" << elapse << std::endl;

	gettimeofday(&tp, NULL);
	start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

	for(int i = 0 ; i < repeat;i++) {
		__m128i r0 = mm_add_epi128_3(a0[i],b0[i]);

		EXPECT_EQ(i, _mm_extract_epi32(r0,0)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r0,1)) << i;
		EXPECT_EQ(0, _mm_extract_epi32(r0,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r0,3)) << i;

		__m128i r1 = mm_add_epi128_3(a1[i],b1[i]);

		EXPECT_EQ(2*i, _mm_extract_epi32(r1,0)) << i;
		EXPECT_EQ(i, _mm_extract_epi32(r1,1)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r1,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r1,3)) << i;

		__m128i r2 = mm_add_epi128_3(a2[i],b2[i]);

		EXPECT_EQ(0, _mm_extract_epi32(r2,0)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r2,1)) << i;
		EXPECT_EQ(1, _mm_extract_epi32(r2,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r2,3)) << i;

	}
	gettimeofday(&tp, NULL);
	elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;
	std::cout << "perf of add 3:" << elapse << std::endl;

	gettimeofday(&tp, NULL);
	start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

	for(int i = 0 ; i < repeat;i++) {
		__m128i r0 = mm_add_epi128(a0[i],b0[i]);

		EXPECT_EQ(i, _mm_extract_epi32(r0,0)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r0,1)) << i;
		EXPECT_EQ(0, _mm_extract_epi32(r0,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r0,3)) << i;

		__m128i r1 = mm_add_epi128(a1[i],b1[i]);

		EXPECT_EQ(2*i, _mm_extract_epi32(r1,0)) << i;
		EXPECT_EQ(i, _mm_extract_epi32(r1,1)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r1,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r1,3)) << i;

		__m128i r2 = mm_add_epi128(a2[i],b2[i]);

		EXPECT_EQ(0, _mm_extract_epi32(r2,0)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r2,1)) << i;
		EXPECT_EQ(1, _mm_extract_epi32(r2,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r2,3)) << i;

	}
	gettimeofday(&tp, NULL);
	elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;
	std::cout << "perf of add 1:" << elapse << std::endl;

	delete[] a0;
	delete[] a1;
	delete[] a2;
	delete[] b0;
	delete[] b1;
	delete[] b2;
}

TEST(Math, TestAddEpi128) {
	for(int i = 0 ; i < 100;i++) {
		__m128i a0 = _mm_setr_epi32(0, 3*i,0,i);
		__m128i b0 = _mm_setr_epi32(i,0,0,2*i);

		__m128i r0 = mm_add_epi128(a0,b0);

		EXPECT_EQ(i, _mm_extract_epi32(r0,0)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r0,1)) << i;
		EXPECT_EQ(0, _mm_extract_epi32(r0,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r0,3)) << i;

		r0 = mm_add_epi128_2(a0,b0);

		EXPECT_EQ(i, _mm_extract_epi32(r0,0)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r0,1)) << i;
		EXPECT_EQ(0, _mm_extract_epi32(r0,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r0,3)) << i;

		r0 = mm_add_epi128_3(a0,b0);

		EXPECT_EQ(i, _mm_extract_epi32(r0,0)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r0,1)) << i;
		EXPECT_EQ(0, _mm_extract_epi32(r0,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r0,3)) << i;

		__m128i a1 = _mm_setr_epi32(0,i,i,i);
		__m128i b1 = _mm_setr_epi32(2*i,0,2*i,2*i);

		__m128i r1 = mm_add_epi128(a1,b1);

		EXPECT_EQ(2*i, _mm_extract_epi32(r1,0)) << i;
		EXPECT_EQ(i, _mm_extract_epi32(r1,1)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r1,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r1,3)) << i;

		r1 = mm_add_epi128_2(a1,b1);

		EXPECT_EQ(2*i, _mm_extract_epi32(r1,0)) << i;
		EXPECT_EQ(i, _mm_extract_epi32(r1,1)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r1,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r1,3)) << i;

		r1 = mm_add_epi128_3(a1,b1);

		EXPECT_EQ(2*i, _mm_extract_epi32(r1,0)) << i;
		EXPECT_EQ(i, _mm_extract_epi32(r1,1)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r1,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r1,3)) << i;

		__m128i a2 = _mm_setr_epi32(0,0x80000000+i,0, i);
		__m128i b2 = _mm_setr_epi32(0,0x80000000+2*i,0, 2*i);

		__m128i r2 = mm_add_epi128(a2,b2);

		EXPECT_EQ(0, _mm_extract_epi32(r2,0)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r2,1)) << i;
		EXPECT_EQ(1, _mm_extract_epi32(r2,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r2,3)) << i;

		r2 = mm_add_epi128_2(a2,b2);

		EXPECT_EQ(0, _mm_extract_epi32(r2,0)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r2,1)) << i;
		EXPECT_EQ(1, _mm_extract_epi32(r2,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r2,3)) << i;

		r2 = mm_add_epi128_3(a2,b2);

		EXPECT_EQ(0, _mm_extract_epi32(r2,0)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r2,1)) << i;
		EXPECT_EQ(1, _mm_extract_epi32(r2,2)) << i;
		EXPECT_EQ(3*i, _mm_extract_epi32(r2,3)) << i;
	}
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
