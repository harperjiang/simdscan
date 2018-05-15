//
// Created by harper on 3/15/18.
//

#include <gtest/gtest.h>
#include "../../../src/util/encode.h"
#include "../../../src/scan/delta/SimdDeltaScanner16.h"


TEST(SimdDeltaScanner16, equal) {
    int entrySize = 13;
    int data[24] = {12, 3, 7, 7, 1, 4, 6, 2, 5, 3, 4,
                    41, 42, 14, 13, 14, 20, 1, 4, 5, 1, 2, 5, 5};
    int *output = new int[30];
    int *cumsum = new int[30];
    cumsum[0] = data[0];
    for (int i = 1; i < 24; i++) {
        cumsum[i] = data[i] + cumsum[i - 1];
    }
    encode(data, output, 24, entrySize);

    SimdDeltaScanner16 *scanner = new SimdDeltaScanner16(entrySize);

    uint8_t *result = new uint8_t[10];
    scanner->scan(output, 24, (int *) result, new Predicate(opr_eq, cumsum[3], 0));

    for (int i = 0; i < 24; i++) {
        if (i != 3)
            EXPECT_EQ(0, result[i / 8] & (1 << (7 - i % 8)));
        else
            EXPECT_EQ(1 << (7 - i % 8), (result[i / 8] & (1 << (7 - i % 8))));
    }

    delete[] cumsum;
    delete[] result;
}

TEST(SimdDeltaScanner16, less) {
    int entrySize = 11;
    int data[24] = {12, 3, 7, 7, 1, 4, 6, 2, 5, 3, 4,
                    4, 2, 1, 3, 4, 2, 1, 4, 5, 1, 2, 5, 5};
    int *output = new int[30];
    int *cumsum = new int[30];
    cumsum[0] = data[0];
    for (int i = 1; i < 24; i++) {
        cumsum[i] = data[i] + cumsum[i - 1];
    }
    encode(data, output, 24, entrySize);

    SimdDeltaScanner16 *scanner = new SimdDeltaScanner16(entrySize);

    uint8_t *result = new uint8_t[10];
    scanner->scan(output, 24, (int *) result, new Predicate(opr_less, 30, 0));

    for (int i = 0; i < 24; i++) {
        if (cumsum[i] < 30)
            EXPECT_EQ(1 << (7 - i % 8), result[i / 8] & (1 << (7 - i % 8))) << i;
        else
            EXPECT_EQ(0, (result[i / 8] & (1 << (7 - i % 8)))) << i;
    }

    delete[] cumsum;
    delete[] result;
}

TEST(SimdDeltaScanner16, cumsum) {

    __m256i input = _mm256_setr_epi16(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);

    SimdDeltaScanner16 *scanner = new SimdDeltaScanner16(29);

    __m256i cumsum = scanner->cumsum(input);

    EXPECT_EQ(_mm256_extract_epi16(cumsum,0), 1);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,1), 3);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,2), 6);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,3), 10);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,4), 15);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,5), 21);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,6), 28);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,7), 36);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,8), 45);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,9), 55);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,10), 66);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,11), 78);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,12), 91);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,13), 105);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,14), 120);
    EXPECT_EQ(_mm256_extract_epi16(cumsum,15), 136);
}