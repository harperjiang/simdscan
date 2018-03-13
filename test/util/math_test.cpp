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

extern __m128i mm_add_epi128_1(__m128i a, __m128i b);

extern __m128i mm_sub_epi128_1(__m128i a, __m128i b);

extern __m256i mm256_add_epi256_1(__m256i a, __m256i b);

extern __m256i mm256_add_epi256_2(__m256i a, __m256i b);

/*
TEST(Math, TestAddEpi128Perf) {

    int repeat = 10000000;
    long start, elapse;

    __m128i *a0 = new __m128i[repeat];
    __m128i *b0 = new __m128i[repeat];

    __m128i *a1 = new __m128i[repeat];
    __m128i *b1 = new __m128i[repeat];

    __m128i *a2 = new __m128i[repeat];
    __m128i *b2 = new __m128i[repeat];

    for (int i = 0; i < repeat; i++) {
        a0[i] = _mm_setr_epi32(0, 3 * i, 0, i);
        b0[i] = _mm_setr_epi32(i, 0, 0, 2 * i);

        a1[i] = _mm_setr_epi32(0, i, i, i);
        b1[i] = _mm_setr_epi32(2 * i, 0, 2 * i, 2 * i);

        a2[i] = _mm_setr_epi32(0, 0x80000000 + i, 0, i);
        b2[i] = _mm_setr_epi32(0, 0x80000000 + 2 * i, 0, 2 * i);
    }

    struct timeval tp;
    gettimeofday(&tp, NULL);
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    for (int i = 0; i < repeat; i++) {
        __m128i r0 = mm_add_epi128(a0[i], b0[i]);

        EXPECT_EQ(i, _mm_extract_epi32(r0, 0)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r0, 1)) << i;
        EXPECT_EQ(0, _mm_extract_epi32(r0, 2)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r0, 3)) << i;

        __m128i r1 = mm_add_epi128(a1[i], b1[i]);

        EXPECT_EQ(2 * i, _mm_extract_epi32(r1, 0)) << i;
        EXPECT_EQ(i, _mm_extract_epi32(r1, 1)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r1, 2)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r1, 3)) << i;

        __m128i r2 = mm_add_epi128(a2[i], b2[i]);

        EXPECT_EQ(0, _mm_extract_epi32(r2, 0)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r2, 1)) << i;
        EXPECT_EQ(1, _mm_extract_epi32(r2, 2)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r2, 3)) << i;

    }
    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;
    std::cout << "perf of add base:" << elapse << std::endl;

    gettimeofday(&tp, NULL);
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    for (int i = 0; i < repeat; i++) {
        __m128i r0 = mm_add_epi128_1(a0[i], b0[i]);

        EXPECT_EQ(i, _mm_extract_epi32(r0, 0)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r0, 1)) << i;
        EXPECT_EQ(0, _mm_extract_epi32(r0, 2)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r0, 3)) << i;

        __m128i r1 = mm_add_epi128_1(a1[i], b1[i]);

        EXPECT_EQ(2 * i, _mm_extract_epi32(r1, 0)) << i;
        EXPECT_EQ(i, _mm_extract_epi32(r1, 1)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r1, 2)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r1, 3)) << i;

        __m128i r2 = mm_add_epi128_1(a2[i], b2[i]);

        EXPECT_EQ(0, _mm_extract_epi32(r2, 0)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r2, 1)) << i;
        EXPECT_EQ(1, _mm_extract_epi32(r2, 2)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r2, 3)) << i;

    }
    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;
    std::cout << "perf of add candidate:" << elapse << std::endl;

    delete[] a0;
    delete[] a1;
    delete[] a2;
    delete[] b0;
    delete[] b1;
    delete[] b2;
}
*/
TEST(Math, TestAddEpi128) {
    for (int i = 0; i < 100; i++) {
        __m128i a0 = _mm_setr_epi32(0, 3 * i, 0, i);
        __m128i b0 = _mm_setr_epi32(i, 0, 0, 2 * i);

        __m128i r0 = mm_add_epi128(a0, b0);

        EXPECT_EQ(i, _mm_extract_epi32(r0, 0)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r0, 1)) << i;
        EXPECT_EQ(0, _mm_extract_epi32(r0, 2)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r0, 3)) << i;

        __m128i a1 = _mm_setr_epi32(0, i, i, i);
        __m128i b1 = _mm_setr_epi32(2 * i, 0, 2 * i, 2 * i);

        __m128i r1 = mm_add_epi128(a1, b1);

        EXPECT_EQ(2 * i, _mm_extract_epi32(r1, 0)) << i;
        EXPECT_EQ(i, _mm_extract_epi32(r1, 1)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r1, 2)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r1, 3)) << i;

        __m128i a2 = _mm_setr_epi32(0, 0x80000000 + i, 0, i);
        __m128i b2 = _mm_setr_epi32(0, 0x80000000 + 2 * i, 0, 2 * i);

        __m128i r2 = mm_add_epi128(a2, b2);

        EXPECT_EQ(0, _mm_extract_epi32(r2, 0)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r2, 1)) << i;
        EXPECT_EQ(1, _mm_extract_epi32(r2, 2)) << i;
        EXPECT_EQ(3 * i, _mm_extract_epi32(r2, 3)) << i;
    }
}

TEST(Math, TestSubEpi128) {

    for (int i = 0; i < 100; i++) {
        __m128i a0 = _mm_setr_epi32(i, 3 * i, i, 2);
        __m128i b0 = _mm_setr_epi32(0, 2 * i, 2 * i, 1);

        __m128i r0 = mm_sub_epi128(a0, b0);

        EXPECT_EQ(i, _mm_extract_epi32(r0, 0)) << i;
        EXPECT_EQ(i, _mm_extract_epi32(r0, 1)) << i;
        EXPECT_EQ(-i, _mm_extract_epi32(r0, 2)) << i;
        EXPECT_EQ(i == 0 ? 1 : 0, _mm_extract_epi32(r0, 3)) << i;

        __m128i a1 = _mm_setr_epi32(0, 0, 2, i);
        __m128i b1 = _mm_setr_epi32(0, i, 1, 0);

        __m128i r1 = mm_sub_epi128(a1, b1);

        EXPECT_EQ(0, _mm_extract_epi32(r1, 0)) << i;
        EXPECT_EQ(-i, _mm_extract_epi32(r1, 1)) << i;
        EXPECT_EQ(i == 0 ? 1 : 0, _mm_extract_epi32(r1, 2)) << i;
        EXPECT_EQ(i, _mm_extract_epi32(r1, 3)) << i;

        __m128i a2 = _mm_setr_epi32(0, i + 1, 0, i + 1);
        __m128i b2 = _mm_setr_epi32(0, 2 * i + 2, 0, 2 * i + 1);

        __m128i r2 = mm_sub_epi128(a2, b2);

        EXPECT_EQ(0, _mm_extract_epi32(r2, 0)) << i;
        EXPECT_EQ(-i - 1, _mm_extract_epi32(r2, 1)) << i;
        EXPECT_EQ(-1, _mm_extract_epi32(r2, 2)) << i;
        EXPECT_EQ(-i - 1, _mm_extract_epi32(r2, 3)) << i;
    }
}
/*
TEST(Math, TestSubEpi128Perf) {

    int repeat = 10000000;
    long start, elapse;

    __m128i *a0 = new __m128i[repeat];
    __m128i *b0 = new __m128i[repeat];

    __m128i *a1 = new __m128i[repeat];
    __m128i *b1 = new __m128i[repeat];

    __m128i *a2 = new __m128i[repeat];
    __m128i *b2 = new __m128i[repeat];

    for (int i = 0; i < repeat; i++) {
        a0[i] = _mm_setr_epi32(i, 3 * i, i, 2);
        b0[i] = _mm_setr_epi32(0, 2 * i, 2 * i, 1);

        a1[i] = _mm_setr_epi32(0, 0, 2, i);
        b1[i] = _mm_setr_epi32(0, i, 1, 0);

        a2[i] = _mm_setr_epi32(0, i + 1, 0, i + 1);
        b2[i] = _mm_setr_epi32(0, 2 * i + 2, 0, 2 * i + 1);
    }

    struct timeval tp;
    gettimeofday(&tp, NULL);
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    for (int i = 0; i < repeat; i++) {
        __m128i r0 = mm_sub_epi128(a0[i], b0[i]);

        EXPECT_EQ(i, _mm_extract_epi32(r0, 0)) << i;
        EXPECT_EQ(i, _mm_extract_epi32(r0, 1)) << i;
        EXPECT_EQ(-i, _mm_extract_epi32(r0, 2)) << i;
        EXPECT_EQ(i == 0 ? 1 : 0, _mm_extract_epi32(r0, 3)) << i;

        __m128i r1 = mm_sub_epi128(a1[i], b1[i]);

        EXPECT_EQ(0, _mm_extract_epi32(r1, 0)) << i;
        EXPECT_EQ(-i, _mm_extract_epi32(r1, 1)) << i;
        EXPECT_EQ(i == 0 ? 1 : 0, _mm_extract_epi32(r1, 2)) << i;
        EXPECT_EQ(i, _mm_extract_epi32(r1, 3)) << i;

        __m128i r2 = mm_sub_epi128(a2[i], b2[i]);

        EXPECT_EQ(0, _mm_extract_epi32(r2, 0)) << i;
        EXPECT_EQ(-i - 1, _mm_extract_epi32(r2, 1)) << i;
        EXPECT_EQ(-1, _mm_extract_epi32(r2, 2)) << i;
        EXPECT_EQ(-i - 1, _mm_extract_epi32(r2, 3)) << i;
    }

    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;
    std::cout << "perf of sub base:" << elapse << std::endl;

    gettimeofday(&tp, NULL);
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    for (int i = 0; i < repeat; i++) {
        __m128i r0 = mm_sub_epi128_1(a0[i], b0[i]);

        EXPECT_EQ(i, _mm_extract_epi32(r0, 0)) << i;
        EXPECT_EQ(i, _mm_extract_epi32(r0, 1)) << i;
        EXPECT_EQ(-i, _mm_extract_epi32(r0, 2)) << i;
        EXPECT_EQ(i == 0 ? 1 : 0, _mm_extract_epi32(r0, 3)) << i;

        __m128i r1 = mm_sub_epi128_1(a1[i], b1[i]);

        EXPECT_EQ(0, _mm_extract_epi32(r1, 0)) << i;
        EXPECT_EQ(-i, _mm_extract_epi32(r1, 1)) << i;
        EXPECT_EQ(i == 0 ? 1 : 0, _mm_extract_epi32(r1, 2)) << i;
        EXPECT_EQ(i, _mm_extract_epi32(r1, 3)) << i;

        __m128i r2 = mm_sub_epi128_1(a2[i], b2[i]);

        EXPECT_EQ(0, _mm_extract_epi32(r2, 0)) << i;
        EXPECT_EQ(-i - 1, _mm_extract_epi32(r2, 1)) << i;
        EXPECT_EQ(-1, _mm_extract_epi32(r2, 2)) << i;
        EXPECT_EQ(-i - 1, _mm_extract_epi32(r2, 3)) << i;
    }

    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;
    std::cout << "perf of sub candidate:" << elapse << std::endl;


    delete[] a0;
    delete[] a1;
    delete[] a2;
    delete[] b0;
    delete[] b1;
    delete[] b2;
}
*/
TEST(Math, TestAddEpi256) {

    long top = 0x8000000000000000;

    for (int i = 0; i < 1000; i++) {
        __m256i a = _mm256_setr_epi64x(i, 0, 0, 0);
        __m256i b = _mm256_setr_epi64x(2 * i, 0, 0, 0);

        __m256i r1 = mm256_add_epi256(a, b);
        EXPECT_EQ(3 * i, _mm256_extract_epi64(r1, 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 3)) << i;

        a = _mm256_setr_epi64x(top + i, 0, 0, 0);
        b = _mm256_setr_epi64x(top + 2 * i, 0, 0, 0);

        __m256i r2 = mm256_add_epi256(a, b);
        EXPECT_EQ(3 * i, _mm256_extract_epi64(r2, 0)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(r2, 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r2, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r2, 3)) << i;

        a = _mm256_setr_epi64x(3 * i, top + i, 0, 0);
        b = _mm256_setr_epi64x(5 * i, top + 2 * i, 0, 0);

        __m256i r3 = mm256_add_epi256(a, b);
        EXPECT_EQ(8 * i, _mm256_extract_epi64(r3, 0)) << i;
        EXPECT_EQ(3 * i, _mm256_extract_epi64(r3, 1)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(r3, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r3, 3)) << i;

        a = _mm256_setr_epi64x(3 * i, top, top + i, 0);
        b = _mm256_setr_epi64x(5 * i, top, top + 2 * i, 0);

        __m256i r4 = mm256_add_epi256(a, b);
        EXPECT_EQ(8 * i, _mm256_extract_epi64(r4, 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r4, 1)) << i;
        EXPECT_EQ(3 * i + 1, _mm256_extract_epi64(r4, 2)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(r4, 3)) << i;
    }
}

TEST(Math, TestAddEpi512) {

    long top = 0x8000000000000000;

    for (int i = 0; i < 3; i++) {
        __m512i a = _mm512_setr_epi64(i, 0, 0, 0, 0, 0, 0, 0);
        __m512i b = _mm512_setr_epi64(2 * i, 0, 0, 0, 0, 0, 0, 0);

        __m512i r1 = mm512_add_epi512(a, b);
        EXPECT_EQ(3 * i, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r1, 0), 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r1, 0), 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r1, 0), 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r1, 0), 3)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r1, 1), 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r1, 1), 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r1, 1), 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r1, 1), 3)) << i;

        a = _mm512_setr_epi64(top + i, 0, 0, 0, 0, 0, 0, 0);
        b = _mm512_setr_epi64(top + 2 * i, 0, 0, 0, 0, 0, 0, 0);

        __m512i r2 = mm512_add_epi512(a, b);
        EXPECT_EQ(3 * i, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r2, 0), 0)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r2, 0), 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r2, 0), 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r2, 0), 3)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r2, 1), 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r2, 1), 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r2, 1), 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r2, 1), 3)) << i;

        a = _mm512_setr_epi64(3 * i, top + i, 0, 0, 0, 0, 0, 0);
        b = _mm512_setr_epi64(5 * i, top + 2 * i, 0, 0, 0, 0, 0, 0);

        __m512i r3 = mm512_add_epi512(a, b);
        EXPECT_EQ(8 * i, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r3, 0), 0)) << i;
        EXPECT_EQ(3 * i, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r3, 0), 1)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r3, 0), 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r3, 0), 3)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r3, 1), 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r3, 1), 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r3, 1), 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r3, 1), 3)) << i;

        a = _mm512_setr_epi64(3 * i, 0, 0, top, 0, 0, top + i, 0);
        b = _mm512_setr_epi64(5 * i, 0, 0, top, 0, 0, top + 2 * i, 0);

        __m512i r4 = mm512_add_epi512(a, b);
        EXPECT_EQ(8 * i, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r4, 0), 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r4, 0), 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r4, 0), 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r4, 0), 3)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r4, 1), 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r4, 1), 1)) << i;
        EXPECT_EQ(3 * i, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r4, 1), 2)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(_mm512_extracti64x4_epi64(r4, 1), 3)) << i;
    }
}

/*
TEST(Math, TestAddEpi256Perf) {
    long top = 0x8000000000000000;

    int repeat = 10000000;
    long start, elapse;

    __m256i *a0 = (__m256i *) aligned_alloc(32, repeat * 32);
    __m256i *b0 = (__m256i *) aligned_alloc(32, repeat * 32);

    __m256i *a1 = (__m256i *) aligned_alloc(32, repeat * 32);
    __m256i *b1 = (__m256i *) aligned_alloc(32, repeat * 32);

    __m256i *a2 = (__m256i *) aligned_alloc(32, repeat * 32);
    __m256i *b2 = (__m256i *) aligned_alloc(32, repeat * 32);

    __m256i *a3 = (__m256i *) aligned_alloc(32, repeat * 32);
    __m256i *b3 = (__m256i *) aligned_alloc(32, repeat * 32);

    for (int i = 0; i < repeat; i++) {
        a0[i] = _mm256_setr_epi64x(i, 0, 0, 0);
        b0[i] = _mm256_setr_epi64x(2 * i, 0, 0, 0);

        a1[i] = _mm256_setr_epi64x(top + i, 0, 0, 0);
        b1[i] = _mm256_setr_epi64x(top + 2 * i, 0, 0, 0);

        a2[i] = _mm256_setr_epi64x(3 * i, top + i, 0, 0);
        b2[i] = _mm256_setr_epi64x(5 * i, top + 2 * i, 0, 0);

        a3[i] = _mm256_setr_epi64x(3 * i, top, top + i, 0);
        b3[i] = _mm256_setr_epi64x(5 * i, top, top + 2 * i, 0);
    }

    struct timeval tp;

    gettimeofday(&tp, NULL);
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    for (int i = 0; i < repeat; i++) {
        __m256i r1 = mm256_add_epi256(a0[i], b0[i]);
        EXPECT_EQ(3 * i, _mm256_extract_epi64(r1, 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 3)) << i;

        __m256i r2 = mm256_add_epi256(a1[i], b1[i]);
        EXPECT_EQ(3 * i, _mm256_extract_epi64(r2, 0)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(r2, 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r2, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r2, 3)) << i;

        __m256i r3 = mm256_add_epi256(a2[i], b2[i]);
        EXPECT_EQ(8 * i, _mm256_extract_epi64(r3, 0)) << i;
        EXPECT_EQ(3 * i, _mm256_extract_epi64(r3, 1)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(r3, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r3, 3)) << i;

        __m256i r4 = mm256_add_epi256(a3[i], b3[i]);
        EXPECT_EQ(8 * i, _mm256_extract_epi64(r4, 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r4, 1)) << i;
        EXPECT_EQ(3 * i + 1, _mm256_extract_epi64(r4, 2)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(r4, 3)) << i;
    }

    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;
    std::cout << "perf of add256:" << elapse << std::endl;
    gettimeofday(&tp, NULL);
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    for (int i = 0; i < repeat; i++) {
        __m256i r1 = mm256_add_epi256_1(a0[i], b0[i]);
        EXPECT_EQ(3 * i, _mm256_extract_epi64(r1, 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 3)) << i;

        __m256i r2 = mm256_add_epi256_1(a1[i], b1[i]);
        EXPECT_EQ(3 * i, _mm256_extract_epi64(r2, 0)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(r2, 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r2, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r2, 3)) << i;

        __m256i r3 = mm256_add_epi256_1(a2[i], b2[i]);
        EXPECT_EQ(8 * i, _mm256_extract_epi64(r3, 0)) << i;
        EXPECT_EQ(3 * i, _mm256_extract_epi64(r3, 1)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(r3, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r3, 3)) << i;

        __m256i r4 = mm256_add_epi256_1(a3[i], b3[i]);
        EXPECT_EQ(8 * i, _mm256_extract_epi64(r4, 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r4, 1)) << i;
        EXPECT_EQ(3 * i + 1, _mm256_extract_epi64(r4, 2)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(r4, 3)) << i;
    }

    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;
    std::cout << "perf of add256 cand1:" << elapse << std::endl;
    gettimeofday(&tp, NULL);
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    for (int i = 0; i < repeat; i++) {
        __m256i r1 = mm256_add_epi256_2(a0[i], b0[i]);
        EXPECT_EQ(3 * i, _mm256_extract_epi64(r1, 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 3)) << i;

        __m256i r2 = mm256_add_epi256_2(a1[i], b1[i]);
        EXPECT_EQ(3 * i, _mm256_extract_epi64(r2, 0)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(r2, 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r2, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r2, 3)) << i;

        __m256i r3 = mm256_add_epi256_2(a2[i], b2[i]);
        EXPECT_EQ(8 * i, _mm256_extract_epi64(r3, 0)) << i;
        EXPECT_EQ(3 * i, _mm256_extract_epi64(r3, 1)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(r3, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r3, 3)) << i;

        __m256i r4 = mm256_add_epi256_2(a3[i], b3[i]);
        EXPECT_EQ(8 * i, _mm256_extract_epi64(r4, 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r4, 1)) << i;
        EXPECT_EQ(3 * i + 1, _mm256_extract_epi64(r4, 2)) << i;
        EXPECT_EQ(1, _mm256_extract_epi64(r4, 3)) << i;
    }

    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;
    std::cout << "perf of add256 cand2:" << elapse << std::endl;

    free(a0);
    free(a1);
    free(a2);
    free(a3);
    free(b0);
    free(b1);
    free(b2);
    free(b3);
}
*/
TEST(Math, TestSubEpi256) {
    long top = 0x8000000000000000;

    for (int i = 1; i < 1000; i++) {
        __m256i a = _mm256_setr_epi64x(2 * i, 0, 0, 0);
        __m256i b = _mm256_setr_epi64x(i, 0, 0, 0);

        __m256i r1 = mm256_sub_epi256(a, b);
        EXPECT_EQ(i, _mm256_extract_epi64(r1, 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r1, 3)) << i;

        a = _mm256_setr_epi64x(0, 1, 0, 0);
        b = _mm256_setr_epi64x(i, 0, 0, 0);

        __m256i r2 = mm256_sub_epi256(a, b);
        EXPECT_EQ(-i, _mm256_extract_epi64(r2, 0)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r2, 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r2, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r2, 3)) << i;

        a = _mm256_setr_epi64x(0, 0, 1, 0);
        b = _mm256_setr_epi64x(i, 0, 0, 0);

        __m256i r3 = mm256_sub_epi256(a, b);
        EXPECT_EQ(-i, _mm256_extract_epi64(r3, 0)) << i;
        EXPECT_EQ(-1, _mm256_extract_epi64(r3, 1)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r3, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r3, 3)) << i;

        a = _mm256_setr_epi64x(i, 3 * i, i, 1);
        b = _mm256_setr_epi64x(2 * i, 5 * i, 3 * i, 0);

        __m256i r4 = mm256_sub_epi256(a, b);
        EXPECT_EQ(-i, _mm256_extract_epi64(r4, 0)) << i;
        EXPECT_EQ(-2 * i - 1, _mm256_extract_epi64(r4, 1)) << i;
        EXPECT_EQ(-2 * i - 1, _mm256_extract_epi64(r4, 2)) << i;
        EXPECT_EQ(0, _mm256_extract_epi64(r4, 3)) << i;
    }
}
/*
TEST(Math, TestSubEpi256Perf) {

}*/
