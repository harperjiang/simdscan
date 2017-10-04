/*
 * HaoScanner_test.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include "../../src/scan/WillhalmScanner256.h"
#include <immintrin.h>
#include <gtest/gtest.h>

//extern __m256i shift_64_256(__m256i, int *);
//
//extern int checkLargeEntry256(int, int);
//
//extern __m256i shuffle64Lane256(__m256i data, int offset, int entrySize, __m256i *shift, __m256i *mask);
//
//extern __m256i shuffleShift32Lane256(__m256i data, int offset, int entrySize,
//                                     __m256i *shift, __m256i *mask);
//
//extern __m256i shuffle32Lane256(__m256i data, int offset, int entrySize, __m256i *shift, __m256i *mask);
//
//extern void encode(int *input, int *output, int length, int entrySize);
//
//TEST(WillhalmScanner256, TestShift64) {
//
//    uint64_t x = 0xfd42324413df2037;
//    uint64_t y = 0x4242442123234d14;
//    uint64_t z = 0xfd435432ab110d12;
//    uint64_t w = 0x12341442342abc15;
//
//    __m256i data = _mm256_setr_epi64x(x, y, z, w);
//
//    int s0[] = {2, 1, 2, 3};
//    __m256i a = shift_64_256(data, s0);
//    ASSERT_EQ(x >> 2, _mm256_extract_epi64(a, 0));
//    ASSERT_EQ(y >> 1, _mm256_extract_epi64(a, 1));
//    ASSERT_EQ(z >> 2, _mm256_extract_epi64(a, 2));
//    ASSERT_EQ(w >> 3, _mm256_extract_epi64(a, 3));
//
//
//    int s1[] = {-2, -1, -3, -5};
//    __m256i b = shift_64_256(data, s1);
//
//    ASSERT_EQ(x << 2, _mm256_extract_epi64(b, 0));
//    ASSERT_EQ(y << 1, _mm256_extract_epi64(b, 1));
//    ASSERT_EQ(z << 3, _mm256_extract_epi64(b, 2));
//    ASSERT_EQ(w << 5, _mm256_extract_epi64(b, 3));
//
//    int s2[] = {-2, 1, 2, -3};
//    __m256i c = shift_64_256(data, s2);
//
//    ASSERT_EQ(x << 2, _mm256_extract_epi64(c, 0));
//    ASSERT_EQ(y >> 1, _mm256_extract_epi64(c, 1));
//    ASSERT_EQ(z >> 2, _mm256_extract_epi64(c, 2));
//    ASSERT_EQ(w << 3, _mm256_extract_epi64(c, 3));
//
//}
//
//TEST(WillhalmScanner256, TestCheckLargeEntry256) {
//    // Check 4 bytes
//    for (int j = 0; j < 26; j++) {
//        for (int i = 0; i < j; i++) {
//            EXPECT_EQ(0, checkLargeEntry256(i, j)) << i << "," << j;
//        }
//    }
//    // Check 5 bytes
//    for (int i = 0; i < 26; i++) {
//        if (i % 2)
//            EXPECT_EQ(1, checkLargeEntry256(i, 26)) << i;
//        else
//            EXPECT_EQ(0, checkLargeEntry256(i, 26)) << i;
//    }
//    for (int i = 0; i < 8; i++) {
//            EXPECT_EQ(1, checkLargeEntry256(i, 27)) << i;
//    }
//    // Check 9 bytes
//    for (int i = 0; i < 8; i++) {
//        if (i == 7 || i == 5 || i == 3 || i == 1)
//            EXPECT_EQ(2, checkLargeEntry256(i, 29)) << i;
//        else {
//            EXPECT_EQ(1, checkLargeEntry256(i, 29)) << i;
//        }
//    }
//}
//
//TEST(WillhalmScanner256, TestShuffle64Lane256) {
//    __m256i data = _mm256_setr_epi64x(0xa0022f38000613d2, 0x0004ba586401d541, 0x32432dabcda32343, 0x4234234542423);
//
//    int offset = 0;
//    int entrySize = 30;
//    __m256i shift;
//    __m256i mask;
//
//    __m256i processed = shuffle64Lane256(data, offset, entrySize, &shift, &mask);
//
//    EXPECT_EQ(offset, 0);
//
//    EXPECT_EQ(0x613d2, _mm256_extract_epi32(processed, 0));
//    EXPECT_EQ(0x8BCE0, _mm256_extract_epi32(processed, 1));
//    EXPECT_EQ(0x1D541A, _mm256_extract_epi32(processed, 2));
//    EXPECT_EQ(0x12E9619, _mm256_extract_epi32(processed, 3));
//    EXPECT_EQ(0x23234300, _mm256_extract_epi32(processed, 4));
//    EXPECT_EQ(0xCB6AF36, _mm256_extract_epi32(processed, 5));
//    EXPECT_EQ(0xC8D0F24, _mm256_extract_epi32(processed, 6));
//    EXPECT_EQ(0x8D08D15, _mm256_extract_epi32(processed, 7));
//
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 0));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 1));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 2));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 3));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 4));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 5));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 6));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 7));
//
//
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 0));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 1));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 2));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 3));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 4));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 5));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 6));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 7));
//
//
//    offset = 7;
//    __m256i data2 = _mm256_setr_epi64x(0x01179c000309e900, 0x25d2c3200eaa0d0, 0x3424523423234, 0x43235243412cabcd);
//    processed = shuffle64Lane256(data2, offset, entrySize, &shift, &mask);
//
//    EXPECT_EQ(0x613D2, _mm256_extract_epi32(processed, 0));
//    EXPECT_EQ(0x8BCE0, _mm256_extract_epi32(processed, 1));
//    EXPECT_EQ(0x1D541A, _mm256_extract_epi32(processed, 2));
//    EXPECT_EQ(0x112E9619, _mm256_extract_epi32(processed, 3));
//    EXPECT_EQ(0x6846468, _mm256_extract_epi32(processed, 4));
//    EXPECT_EQ(0x269A1229, _mm256_extract_epi32(processed, 5));
//    EXPECT_EQ(0x6825957, _mm256_extract_epi32(processed, 6));
//    EXPECT_EQ(0x2191A92, _mm256_extract_epi32(processed, 7));
//
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 0));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 1));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 2));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 3));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 4));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 5));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 6));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 7));
//
//
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 0));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 1));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 2));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 3));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 4));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 5));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 6));
//    EXPECT_EQ(0x3fffffff, _mm256_extract_epi32(mask, 7));
//}
//
//TEST(WillhalmScanner256, TestShuffleShift32Lane) {
//    __m256i data = _mm256_setr_epi64x(0x2352423309e900, 0x1031414d01179c0, 0x1341241c320eaa0, 0x432432452425d2);
//    int offset = 7;
//    int entrySize = 26;
//    __m256i shift;
//    __m256i mask;
//
//    __m256i processed = shuffleShift32Lane256(data, offset, entrySize, &shift,
//                                              &mask);
//
//    EXPECT_EQ(0x6613D2 << 6, _mm256_extract_epi32(processed, 0));
//    EXPECT_EQ(0x11A921, _mm256_extract_epi32(processed, 1));
//    EXPECT_EQ(0x2022F38 << 6, _mm256_extract_epi32(processed, 2));
//    EXPECT_EQ(0x18A0A6, _mm256_extract_epi32(processed, 3));
//    EXPECT_EQ(0x1540206 << 6, _mm256_extract_epi32(processed, 4));
//    EXPECT_EQ(0x20E1907, _mm256_extract_epi32(processed, 5));
//    EXPECT_EQ(0x268248 << 6, _mm256_extract_epi32(processed, 6));
//    EXPECT_EQ(0x29212E9, _mm256_extract_epi32(processed, 7));
//
//    EXPECT_EQ(6, _mm256_extract_epi32(shift, 0));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 1));
//    EXPECT_EQ(6, _mm256_extract_epi32(shift, 2));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 3));
//    EXPECT_EQ(6, _mm256_extract_epi32(shift, 4));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 5));
//    EXPECT_EQ(6, _mm256_extract_epi32(shift, 6));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 7));
//
//
//    EXPECT_EQ((int) 0xffffffc0, _mm256_extract_epi32(mask, 0));
//    EXPECT_EQ(0x3ffffff, _mm256_extract_epi32(mask, 1));
//    EXPECT_EQ((int) 0xffffffc0, _mm256_extract_epi32(mask, 2));
//    EXPECT_EQ(0x3ffffff, _mm256_extract_epi32(mask, 3));
//    EXPECT_EQ((int) 0xffffffc0, _mm256_extract_epi32(mask, 4));
//    EXPECT_EQ(0x3ffffff, _mm256_extract_epi32(mask, 5));
//    EXPECT_EQ((int) 0xffffffc0, _mm256_extract_epi32(mask, 6));
//    EXPECT_EQ(0x3ffffff, _mm256_extract_epi32(mask, 7));
//}
//
//TEST(WillhalmScanner256, TestShuffle32Lane) {
//    __m256i data = _mm256_setr_epi64x(0xa234324323824526, 0xabcdeee52cfc032, 0x139523423980f1db, 0x4fda234234143742);
//    int offset = 37;
//    int entrySize = 17;
//    __m256i shift;
//    __m256i mask;
//
//    __m256i processed = shuffle32Lane256(data, offset, entrySize, &shift, &mask);
//
//
//    EXPECT_EQ(0x1A192, _mm256_extract_epi32(processed, 0));
//    EXPECT_EQ(0xCA88, _mm256_extract_epi32(processed, 1));
//    EXPECT_EQ(0x19F80, _mm256_extract_epi32(processed, 2));
//    EXPECT_EQ(0xEE52, _mm256_extract_epi32(processed, 3));
//    EXPECT_EQ(0x15E6F, _mm256_extract_epi32(processed, 4));
//    EXPECT_EQ(0x76C2, _mm256_extract_epi32(processed, 5));
//    EXPECT_EQ(0x1301E, _mm256_extract_epi32(processed, 6));
//    EXPECT_EQ(0x3423, _mm256_extract_epi32(processed, 7));
//
//
//    EXPECT_EQ(5, _mm256_extract_epi32(shift, 0));
//    EXPECT_EQ(6, _mm256_extract_epi32(shift, 1));
//    EXPECT_EQ(7, _mm256_extract_epi32(shift, 2));
//    EXPECT_EQ(0, _mm256_extract_epi32(shift, 3));
//    EXPECT_EQ(1, _mm256_extract_epi32(shift, 4));
//    EXPECT_EQ(2, _mm256_extract_epi32(shift, 5));
//    EXPECT_EQ(3, _mm256_extract_epi32(shift, 6));
//    EXPECT_EQ(4, _mm256_extract_epi32(shift, 7));
//
//
//    EXPECT_EQ(0x1ffff << 5, _mm256_extract_epi32(mask, 0));
//    EXPECT_EQ(0x1ffff << 6, _mm256_extract_epi32(mask, 1));
//    EXPECT_EQ(0x1ffff << 7, _mm256_extract_epi32(mask, 2));
//    EXPECT_EQ(0x1ffff, _mm256_extract_epi32(mask, 3));
//    EXPECT_EQ(0x1ffff << 1, _mm256_extract_epi32(mask, 4));
//    EXPECT_EQ(0x1ffff << 2, _mm256_extract_epi32(mask, 5));
//    EXPECT_EQ(0x1ffff << 3, _mm256_extract_epi32(mask, 6));
//    EXPECT_EQ(0x1ffff << 4, _mm256_extract_epi32(mask, 7));
//}
//
//TEST(WillhalmScanner256, TestScanUnaligned) {
//
//    int entrySize = 9;
//    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
//                  18, 19, 20};
//    int result[8];
//    int output[30];
//    encode(data, result, 20, entrySize);
//
//    WillhalmScanner256 *scanner = new WillhalmScanner256(entrySize, false);
//
//    Predicate p(opr_eq, 18, 0);
//
//    scanner->scan(result, 20, output, &p);
//    for (int i = 0; i < 20; i++) {
//        if (i == 17)
//            EXPECT_EQ((int) 0xffffffff, output[i]) << i;
//        else
//            EXPECT_EQ(0, output[i]) << i;
//    }
//
//    Predicate p2(opr_in, 3, 7);
//
//    scanner->scan(result, 20, output, &p2);
//    for (int i = 0; i < 20; i++) {
//        if (i >= 2 && i <= 6)
//            EXPECT_EQ((int) 0xffffffff, output[i]) << i;
//        else
//            EXPECT_EQ(0, output[i]) << i;
//    }
//}
//
//TEST(WillhalmScanner256, TestScanAligned) {
//
//    int entrySize = 9;
//    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
//                  18, 19, 20};
//    int *encoded = (int *) aligned_alloc(16, 8 * sizeof(int));
//    int *output = (int *) aligned_alloc(16, 32 * sizeof(int));
//    encode(data, encoded, 20, entrySize);
//
//    WillhalmScanner256 *scanner = new WillhalmScanner256(entrySize, true);
//
//    Predicate p(opr_eq, 18, 0);
//
//    scanner->scan(encoded, 20, output, &p);
//    for (int i = 0; i < 20; i++) {
//        if (i == 17)
//            EXPECT_EQ((int) 0xffffffff, output[i]) << i;
//        else
//            EXPECT_EQ(0, output[i]) << i;
//    }
//
//    Predicate p2(opr_in, 3, 7);
//
//    scanner->scan(encoded, 20, output, &p2);
//    for (int i = 0; i < 20; i++) {
//        if (i >= 2 && i <= 6)
//            EXPECT_EQ((int) 0xffffffff, output[i]) << i;
//        else
//            EXPECT_EQ(0, output[i]) << i;
//    }
//    free(encoded);
//    free(output);
//}
