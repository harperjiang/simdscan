/*
 * HaoScanner_test.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include "../../src/scan/WillhalmScanner128.h"
#include <immintrin.h>
#include <gtest/gtest.h>

extern int computeAlign(int residue);

extern int computeShuffle(int* shuffleIdx, int* maskIdx, int* shiftIdx,
		int offset, int entrySize);

TEST(WillhalmScanner128, TestComputeShuffle) {

	int shuffleIdx[16];
	int maskIdx[16];
	int shiftIdx[4];

	computeShuffle(shuffleIdx, maskIdx, shiftIdx, 0, 15);

	int shuffleExpect[] = { 0, 1, 0xff, 0xff, 1, 2, 0xff, 0xff, 2, 3, 0xff,
			0xff, 3, 4, 0xff, 0xff };
	int maskExpect[] = { 0xff, 0x7f, 0, 0, 0x80, 0xff, 0x3f, 0, 0xc0, 0xff,
			0x1f, 0, 0xe0, 0xff, 0xf, 0 };
	int shiftExpect[] = { 0, 7, 6, 5 };

	for (int i = 0; i < 16; i++) {
		ASSERT_EQ(shuffleExpect[i], shuffleIdx[i]);
		ASSERT_EQ(maskExpect[i], maskIdx[i]);
	}

	for (int i = 0; i < 4; i++) {
		ASSERT_EQ(shiftExpect[i], shiftIdx[i]);
	}

	computeShuffle(shuffleIdx, maskIdx, shiftIdx, 6, 14);

	int shuffleExpect2[] = {0, 1, 0xff, 0xff, 1, 2, 0xff, 0xff, 2, 3, 0xff,
		0xff, 3, 4, 0xff, 0xff};
	int maskExpect2[] = {0xff, 0x7f, 0, 0, 0x80, 0xff, 0x3f, 0, 0xc0, 0xff,
		0x1f, 0, 0xe0, 0xff, 0xf, 0};
	int shiftExpect2[] = {0, 7, 6, 5};

	for (int i = 0; i < 16; i++) {
		ASSERT_EQ(shuffleExpect2[i], shuffleIdx[i]);
		ASSERT_EQ(maskExpect2[i], maskIdx[i]);
	}

	for (int i = 0; i < 4; i++) {
		ASSERT_EQ(shiftExpect2[i], shiftIdx[i]);
	}
}

