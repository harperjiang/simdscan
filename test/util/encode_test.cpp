/*
 * encode_test.cpp
 *
 *  Created on: Sep 4, 2017
 *      Author: harper
 */
#include <gtest/gtest.h>

extern void encode(int*, int*, int, int);


TEST(Encode, TestEncode) {

	int input[] = {3,29,13,26,15,17,11};
	int output[2];
	int entrySize = 5;

	encode(input, output, 7, entrySize);

	EXPECT_EQ(0xe2fd37a3, output[0]);
	EXPECT_EQ(2, output[1]);
}
