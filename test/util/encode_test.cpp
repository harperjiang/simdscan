/*
 * encode_test.cpp
 *
 *  Created on: Sep 4, 2017
 *      Author: harper
 */
#include <gtest/gtest.h>

extern void encode(int *, int *, int, int);

extern void bitweaverh_encode(int *, int *, int, int);

TEST(Encode, TestEncode) {

    int input[] = {3, 29, 13, 26, 15, 17, 11};
    int output[2];
    int entrySize = 5;

    encode(input, output, 7, entrySize);

    EXPECT_EQ(0xe2fd37a3, output[0]);
    EXPECT_EQ(2, output[1]);

    int input2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    int output2[6];
    entrySize = 9;

    encode(input2, output2, 20, entrySize);

    EXPECT_EQ(0xc1c0d060, output2[3]);
    EXPECT_EQ(0x24110803, output2[4]);
}

TEST(Encode, TestBitWeaverHEncode) {
    int input[] = {31, 20, 19, 25, 6, 17, 30, 9, 29, 10, 8, 26, 3, 1, 28};
    int output[4];
    int entrySize = 5;

    bitweaverh_encode(input, output, 15, entrySize);

/*
 *  01 1100 0000 0100 0011 0110 1000   1000 0010 1001 1101 0010 0101 1110 0100 0100 0110 0110 0101 0011 0101 0001 1111
 */
    EXPECT_EQ(0x4665351f, output[0]);
    EXPECT_EQ(0x82ad25e4, output[1]) ;
    EXPECT_EQ(0x1c04368, output[2]);
}
