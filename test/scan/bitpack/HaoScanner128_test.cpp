/*
 * HaoScanner_test.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include "../../../src/scan/bitpack/HaoScanner128.h"
#include <immintrin.h>
#include <gtest/gtest.h>

extern __m128i build128(int num, int bitLength, int);

extern __m128i buildMask128(int bitLength, int);

extern int buildPiece128(__m128i, __m128i, int, int);

extern void encode(int *, int *, int, int);

TEST(HaoScanner128, TestBuild128) {
    __m128i b = build128(3, 5, 0);

    ASSERT_EQ(0xc6318c63, _mm_extract_epi32(b, 0));
    ASSERT_EQ(0x318c6318, _mm_extract_epi32(b, 1));
    ASSERT_EQ(0x8c6318c6, _mm_extract_epi32(b, 2));
    ASSERT_EQ(0x6318c631, _mm_extract_epi32(b, 3));

    b = build128(26, 5, 0);

    ASSERT_EQ(0xb5ad6b5a, _mm_extract_epi32(b, 0));
    ASSERT_EQ(0xad6b5ad6, _mm_extract_epi32(b, 1));
    ASSERT_EQ(0x6b5ad6b5, _mm_extract_epi32(b, 2));
    ASSERT_EQ(0x5ad6b5ad, _mm_extract_epi32(b, 3));

    b = build128(3, 5, 2);

    ASSERT_EQ(0x18c6318c, _mm_extract_epi32(b, 0));
    ASSERT_EQ(0xc6318c63, _mm_extract_epi32(b, 1));
    ASSERT_EQ(0x318c6318, _mm_extract_epi32(b, 2));
    ASSERT_EQ(0x8c6318c6, _mm_extract_epi32(b, 3));
}

TEST(HaoScanner128, TestBuildMask128) {
    __m128i b = buildMask128(5, 0);

    ASSERT_EQ(0x21084210, _mm_extract_epi32(b, 0));
    ASSERT_EQ(0x08421084, _mm_extract_epi32(b, 1));
    ASSERT_EQ(0x42108421, _mm_extract_epi32(b, 2));
    ASSERT_EQ(0x10842108, _mm_extract_epi32(b, 3));
}

TEST(HaoScanner128, TestBuildPiece128) {
    __m128i prev = _mm_setr_epi32(0x323423aa, 0x214424aa, 0x4ba3d319, 0xd7235273);
    __m128i current = _mm_setr_epi32(0x32342a4a, 0x234abcd1, 0x12312341, 0x11ddaad5);
    int entrySize = 9;
    int bitOffset = 4;

    EXPECT_EQ(346, buildPiece128(prev, current, entrySize, bitOffset));
}

TEST(HaoScanner128, TestUnalignedEq) {
    int entrySize = 9;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                  18, 19, 20};
    int result[6];
    int output[] = {0, 0, 0, 0, 0, 0};
    encode(data, result, 20, entrySize);

    HaoScanner128 *scanner = new HaoScanner128(entrySize, false);

    Predicate p(opr_eq, 18, 0);

    scanner->scan(result, 20, output, &p);
    for (int i = 0; i < 20; i++) {
        int bitOffset = (i + 1) * entrySize - 1;
        int intIdx = bitOffset / 32;
        int bitIdx = bitOffset % 32;
        if (i == 17) {
            EXPECT_EQ(0, output[intIdx] & (1 << bitIdx)) << i;
        } else {
            EXPECT_EQ(1 << bitIdx, output[intIdx] & (1 << bitIdx)) << i;
        }
    }
}

TEST(HaoScanner128, TestAlignedEq) {
    int entrySize = 9;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                  18, 19, 20};
    int *encoded = (int *) aligned_alloc(16, 8 * sizeof(int));
    int *output = (int *) aligned_alloc(16, 8 * sizeof(int));
    encode(data, encoded, 20, entrySize);

    HaoScanner128 *scanner = new HaoScanner128(entrySize, true);

    Predicate p(opr_eq, 18, 0);

    scanner->scan(encoded, 20, output, &p);
    for (int i = 0; i < 20; i++) {
        int bitOffset = (i + 1) * entrySize - 1;
        int intIdx = bitOffset / 32;
        int bitIdx = bitOffset % 32;
        if (i == 17) {
            EXPECT_EQ(0, output[intIdx] & (1 << bitIdx)) << i;
        } else {
            EXPECT_EQ(1 << bitIdx, output[intIdx] & (1 << bitIdx)) << i;
        }
    }
    free(encoded);
    free(output);
}

TEST(HaoScanner128, TestUnalignedRange) {
    int entrySize = 9;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                  18, 19, 20};
    int result[3];
    int output[30];
    encode(data, result, 20, entrySize);

    HaoScanner128 *scanner = new HaoScanner128(entrySize, false);

    Predicate p(opr_in, 3, 8);

    scanner->scan(result, 20, output, &p);
    for (int i = 0; i < 20; i++) {
        int bitOffset = (i + 1) * entrySize - 1;
        int intIdx = bitOffset / 32;
        int bitIdx = bitOffset % 32;
        if (i >= 2 && i <= 6)
            EXPECT_EQ(1 << bitIdx, output[intIdx] & (1 << bitIdx)) << i;
        else
            EXPECT_EQ(0, output[intIdx] & (1 << bitIdx)) << i;
    }
}

TEST(HaoScanner128, TestAlignedRange) {
    int entrySize = 9;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                  18, 19, 20};
    int *encoded = (int *) aligned_alloc(16, 8 * sizeof(int));
    int *output = (int *) aligned_alloc(16, 8 * sizeof(int));
    encode(data, encoded, 20, entrySize);

    HaoScanner128 *scanner = new HaoScanner128(entrySize, true);

    Predicate p(opr_in, 3, 8);

    scanner->scan(encoded, 20, output, &p);
    for (int i = 0; i < 20; i++) {
        int bitOffset = (i + 1) * entrySize - 1;
        int intIdx = bitOffset / 32;
        int bitIdx = bitOffset % 32;
        if (i >= 2 && i <= 6)
            EXPECT_EQ(1 << bitIdx, output[intIdx] & (1 << bitIdx)) << i;
        else
            EXPECT_EQ(0, output[intIdx] & (1 << bitIdx)) << i;
    }
    free(encoded);
    free(output);
}