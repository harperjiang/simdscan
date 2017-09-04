/*
 * HaoScanner_test.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include "../../src/scan/WillhalmScanner128.h"
#include <immintrin.h>
#include <gtest/gtest.h>

extern __m128i shift_64(__m128i, int, int);
extern int checkLargeEntry(int, int);
extern __m128i shuffle64Lane(__m128i data, int* offset, int entrySize, __m128i* shift, __m128i* mask);
extern __m128i shuffleShift32Lane(__m128i data, int* offset, int entrySize,
		__m128i* shift, __m128i* mask);
extern __m128i shuffle32Lane(__m128i data, int* offset, int entrySize, __m128i* shift, __m128i* mask);
extern __m128i shuffle(__m128i data, int* offset, int entrySize, __m128i* shift, __m128i* mask);

extern int* encode(int* input, int* output, int length, int entrySize);

TEST(WillhalmScanner128, TestShift64) {

	__m64 f = _mm_set_pi64x(54l);
	__m64 s = _mm_set_pi64x(32l);
	__m128i data = _mm_setr_epi64(f, s);

	__m128i a = shift_64(data, 2, 4);
	ASSERT_EQ(54l >> 2, _mm_extract_epi64(a,0));
	ASSERT_EQ(32l >> 4, _mm_extract_epi64(a,1));

	__m128i b = shift_64(data, -3, -5);

	ASSERT_EQ(54l << 3, _mm_extract_epi64(b,0));
	ASSERT_EQ(32l << 5, _mm_extract_epi64(b,1));

	__m128i c = shift_64(data, 7, -9);

	ASSERT_EQ(54l >> 7, _mm_extract_epi64(c,0));
	ASSERT_EQ(32l << 9, _mm_extract_epi64(c,1));

}

TEST(WillhalmScanner128, TestCheckLargeEntry) {
	// Check 4 bytes
	for (int j = 0; j < 26; j++) {
		for (int i = 0; i < j; i++) {
			EXPECT_EQ(0, checkLargeEntry(i, j)) << i << "," << j;
		}
	}
	// Check 5 bytes
	for (int i = 0; i < 26; i++) {
		if (i % 2)
			EXPECT_EQ(1, checkLargeEntry(i, 26)) << i;
		else
			EXPECT_EQ(0, checkLargeEntry(i, 26)) << i;
	}
	for (int i = 0; i < 8; i++) {
		if (i == 2) {
			EXPECT_EQ(0, checkLargeEntry(i,27)) << i;
		} else {
			EXPECT_EQ(1, checkLargeEntry(i,27)) << i;
		}
	}
	// Check 9 bytes
	for (int i = 0; i < 8; i++) {
		if (i == 7 || i == 5)
			EXPECT_EQ(2, checkLargeEntry(i,29)) << i;
		else {
			EXPECT_EQ(1, checkLargeEntry(i,29)) << i;
		}
	}
}

TEST(WillhalmScanner128, TestShuffle64Lane) {
	__m128i data = _mm_setr_epi64(_mm_set_pi64x(0xa0022f38000613d2),
			_mm_set_pi64x(0x0004ba586401d541));
	int offset = 0;
	int entrySize = 30;
	__m128i shift;
	__m128i mask;

	__m128i processed = shuffle64Lane(data, &offset, entrySize, &shift, &mask);

	EXPECT_EQ(offset, 120);

	EXPECT_EQ(0x613d2, _mm_extract_epi32(processed,0));
	EXPECT_EQ(0x8bce0, _mm_extract_epi32(processed,1));
	EXPECT_EQ(0x1d541a, _mm_extract_epi32(processed,2));
	EXPECT_EQ(0x12e9619, _mm_extract_epi32(processed,3));

	EXPECT_EQ(0, _mm_extract_epi32(shift,0));
	EXPECT_EQ(0, _mm_extract_epi32(shift,1));
	EXPECT_EQ(0, _mm_extract_epi32(shift,2));
	EXPECT_EQ(0, _mm_extract_epi32(shift,3));

	EXPECT_EQ(0x3fffffff, _mm_extract_epi32(mask,0));
	EXPECT_EQ(0x3fffffff, _mm_extract_epi32(mask,1));
	EXPECT_EQ(0x3fffffff, _mm_extract_epi32(mask,2));
	EXPECT_EQ(0x3fffffff, _mm_extract_epi32(mask,3));

	offset = 7;
	__m128i data2 = _mm_setr_epi64(_mm_set_pi64x(0x01179c000309e900),
			_mm_set_pi64x(0x25d2c3200eaa0d0));
	processed = shuffle64Lane(data2, &offset, entrySize, &shift, &mask);

	EXPECT_EQ(offset, 127);

	EXPECT_EQ(0x613d2, _mm_extract_epi32(processed,0));
	EXPECT_EQ(0x8bce0, _mm_extract_epi32(processed,1));
	EXPECT_EQ(0x1d541a, _mm_extract_epi32(processed,2));
	EXPECT_EQ(0x12e9619, _mm_extract_epi32(processed,3));

	EXPECT_EQ(0, _mm_extract_epi32(shift,0));
	EXPECT_EQ(0, _mm_extract_epi32(shift,1));
	EXPECT_EQ(0, _mm_extract_epi32(shift,2));
	EXPECT_EQ(0, _mm_extract_epi32(shift,3));

	EXPECT_EQ(0x3fffffff, _mm_extract_epi32(mask,0));
	EXPECT_EQ(0x3fffffff, _mm_extract_epi32(mask,1));
	EXPECT_EQ(0x3fffffff, _mm_extract_epi32(mask,2));
	EXPECT_EQ(0x3fffffff, _mm_extract_epi32(mask,3));
}

TEST(WillhalmScanner128, TestShuffleShift32Lane) {
	__m128i data = _mm_setr_epi32(0x309e900, 0xd01179c0, 0xc320eaa0, 0x25d2);
	int offset = 7;
	int entrySize = 26;
	__m128i shift;
	__m128i mask;

	__m128i processed = shuffleShift32Lane(data, &offset, entrySize, &shift,
			&mask);

	EXPECT_EQ(offset, 111);

	EXPECT_EQ(0x613d2 << 6, _mm_extract_epi32(processed,0));
	EXPECT_EQ(0x8bce0, _mm_extract_epi32(processed,1));
	EXPECT_EQ(0x1d541a << 6, _mm_extract_epi32(processed,2));
	EXPECT_EQ(0x12e9619, _mm_extract_epi32(processed,3));

	EXPECT_EQ(6, _mm_extract_epi32(shift,0));
	EXPECT_EQ(0, _mm_extract_epi32(shift,1));
	EXPECT_EQ(6, _mm_extract_epi32(shift,2));
	EXPECT_EQ(0, _mm_extract_epi32(shift,3));

	EXPECT_EQ((int )0xffffffc0, _mm_extract_epi32(mask,0));
	EXPECT_EQ(0x3ffffff, _mm_extract_epi32(mask,1));
	EXPECT_EQ((int )0xffffffc0, _mm_extract_epi32(mask,2));
	EXPECT_EQ(0x3ffffff, _mm_extract_epi32(mask,3));
}

TEST(WillhalmScanner128, TestShuffle32Lane) {
	__m128i data = _mm_setr_epi32(0x38245126, 0xee52cfc0, 0x1980f1db, 0x3742);
	int offset = 37;
	int entrySize = 17;
	__m128i shift;
	__m128i mask;

	__m128i processed = shuffle32Lane(data, &offset, entrySize, &shift, &mask);

	EXPECT_EQ(offset, 105);

	EXPECT_EQ(0x967e << 5, _mm_extract_epi32(processed,0));
	EXPECT_EQ(0x16fb9 << 6, _mm_extract_epi32(processed,1));
	EXPECT_EQ(0x101e3 << 7, _mm_extract_epi32(processed,2));
	EXPECT_EQ(0x14219, _mm_extract_epi32(processed,3));

	EXPECT_EQ(5, _mm_extract_epi32(shift,0));
	EXPECT_EQ(6, _mm_extract_epi32(shift,1));
	EXPECT_EQ(7, _mm_extract_epi32(shift,2));
	EXPECT_EQ(0, _mm_extract_epi32(shift,3));

	EXPECT_EQ(0x1ffff << 5, _mm_extract_epi32(mask,0));
	EXPECT_EQ(0x1ffff << 6, _mm_extract_epi32(mask,1));
	EXPECT_EQ(0x1ffff << 7, _mm_extract_epi32(mask,2));
	EXPECT_EQ(0x1ffff, _mm_extract_epi32(mask,3));
}

TEST(WillhalmScanner128, TestScanUnaligned) {

	int entrySize = 9;
	int data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 16, 17,
			18, 19, 20 };
	int result[3];
	int output[12];
	encode(data, result, 20, entrySize);

	WillhalmScanner128* scanner = new WillhalmScanner128(entrySize);

	Predicate p(opr_eq, 18, 0);

	scanner->scan(result, 10, output, &p);
	for (int i = 0; i < 10; i++) {
		if (i == 17)
			EXPECT_EQ((int )0xffffffff, output[i]);
		else
			EXPECT_EQ(0, output[i]);
	}
}

