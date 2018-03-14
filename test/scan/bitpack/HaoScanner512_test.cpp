/*
 * HaoScanner256_test.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include "../../../src/scan/bitpack/HaoScanner512.h"
#include <immintrin.h>
#include <gtest/gtest.h>

extern __m512i build512(int num, int bitLength, int);

extern __m512i buildMask512(int bitLength, int);

extern void encode(int *, int *, int, int);

TEST(HaoScanner512, TestBuild) {
    __m512i b = build512(3, 5, 0);

    __m256i b1 = _mm512_extracti64x4_epi64(b,0);

    ASSERT_EQ(0xc6318c63, _mm256_extract_epi32(b1, 0));
    ASSERT_EQ(0x318c6318, _mm256_extract_epi32(b1, 1));
    ASSERT_EQ(0x8c6318c6, _mm256_extract_epi32(b1, 2));
    ASSERT_EQ(0x6318c631, _mm256_extract_epi32(b1, 3));
    ASSERT_EQ(0x18c6318c, _mm256_extract_epi32(b1, 4));
    ASSERT_EQ(0xc6318c63, _mm256_extract_epi32(b1, 5));
    ASSERT_EQ(0x318c6318, _mm256_extract_epi32(b1, 6));
    ASSERT_EQ(0x8c6318c6, _mm256_extract_epi32(b1, 7));

    b = build512(26, 5, 0);
    b1 = _mm512_extracti64x4_epi64(b,0);

    ASSERT_EQ(0xb5ad6b5a, _mm256_extract_epi32(b1, 0));
    ASSERT_EQ(0xad6b5ad6, _mm256_extract_epi32(b1, 1));
    ASSERT_EQ(0x6b5ad6b5, _mm256_extract_epi32(b1, 2));
    ASSERT_EQ(0x5ad6b5ad, _mm256_extract_epi32(b1, 3));
    ASSERT_EQ(0xd6b5ad6b, _mm256_extract_epi32(b1, 4));
    ASSERT_EQ(0xb5ad6b5a, _mm256_extract_epi32(b1, 5));
    ASSERT_EQ(0xad6b5ad6, _mm256_extract_epi32(b1, 6));
    ASSERT_EQ(0x6b5ad6b5, _mm256_extract_epi32(b1, 7));
}

TEST(HaoScanner512, TestBuildMask) {
    __m512i b = buildMask512(5, 0);
    __m256i b1 = _mm512_extracti64x4_epi64(b,0);

    ASSERT_EQ(0x21084210, _mm256_extract_epi32(b1, 0));
    ASSERT_EQ(0x08421084, _mm256_extract_epi32(b1, 1));
    ASSERT_EQ(0x42108421, _mm256_extract_epi32(b1, 2));
    ASSERT_EQ(0x10842108, _mm256_extract_epi32(b1, 3));
    ASSERT_EQ(0x84210842, _mm256_extract_epi32(b1, 4));
    ASSERT_EQ(0x21084210, _mm256_extract_epi32(b1, 5));
    ASSERT_EQ(0x08421084, _mm256_extract_epi32(b1, 6));
    ASSERT_EQ(0x42108421, _mm256_extract_epi32(b1, 7));
}

TEST(HaoScanner512, TestUnalignedEq) {
    int entrySize = 9;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                  18, 19, 20};
    int result[6];
    int output[] = {0, 0, 0, 0, 0, 0};
    encode(data, result, 20, entrySize);

    HaoScanner512 *scanner = new HaoScanner512(entrySize, false);

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

TEST(HaoScanner512, TestAlignedEq) {
    int entrySize = 9;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                  18, 19, 20};
    int *encoded = (int *) aligned_alloc(64, 32 * sizeof(int));
    int *output = (int *) aligned_alloc(64, 32 * sizeof(int));
    encode(data, encoded, 20, entrySize);

    HaoScanner512 *scanner = new HaoScanner512(entrySize, true);

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

TEST(HaoScanner512, TestUnalignedLess) {
    int entrySize = 9;
    int data[] = {3, 4, 9, 10, 22, 1, 7, 6, 8, 5, 6, 11, 12, 13, 14, 7, 8, 9, 10, 15, 16, 17,
                  18, 19, 20};
    int *encoded = (int *) aligned_alloc(32, 8 * sizeof(int));
    int *output = (int *) aligned_alloc(32, 8 * sizeof(int));
    encode(data, encoded, 25, entrySize);

    HaoScanner512 *scanner = new HaoScanner512(entrySize, false);

    Predicate p(opr_less, 8, 0);

    scanner->scan(encoded, 25, output, &p);
    for (int i = 0; i < 25; i++) {
        int bitOffset = (i + 1) * entrySize - 1;
        int intIdx = bitOffset / 32;
        int bitIdx = bitOffset % 32;
        if (data[i] >= 8)
            EXPECT_EQ(1 << bitIdx, output[intIdx] & (1 << bitIdx)) << i;
        else
            EXPECT_EQ(0, output[intIdx] & (1 << bitIdx)) << i;
    }
    free(encoded);
    free(output);
}

TEST(HaoScanner512, TestAlignedLess) {
    int entrySize = 9;
    int data[] = {3, 4, 9, 10, 22, 1, 7, 6, 8, 5, 6, 11, 12, 13, 14, 7, 8, 9, 10, 15, 16, 17,
                  18, 19, 20};
    int *encoded = (int *) aligned_alloc(64, 8 * sizeof(int));
    int *output = (int *) aligned_alloc(64, 8 * sizeof(int));
    encode(data, encoded, 25, entrySize);

    HaoScanner512 *scanner = new HaoScanner512(entrySize, true);

    Predicate p(opr_less, 8, 0);

    scanner->scan(encoded, 25, output, &p);
    for (int i = 0; i < 25; i++) {
        int bitOffset = (i + 1) * entrySize - 1;
        int intIdx = bitOffset / 32;
        int bitIdx = bitOffset % 32;
        if (data[i] >= 8)
            EXPECT_EQ(1 << bitIdx, output[intIdx] & (1 << bitIdx)) << i;
        else
            EXPECT_EQ(0, output[intIdx] & (1 << bitIdx)) << i;
    }
    free(encoded);
    free(output);
}