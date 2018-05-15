//
// Created by harper on 3/15/18.
//

#include <gtest/gtest.h>
#include "../../../src/predicate/Predicate.h"
#include "../../../src/scan/delta/SimdDeltaScanner32.h"
#include "../../../src/util/encode.h"

TEST(SimdDeltaScanner32, equal) {
    int entrySize = 20;
    int data[24] = {12, 3, 7, 7, 1, 4, 6, 2, 5, 3, 4,
                    41, 42, 14, 13, 14, 20, 1, 4, 5, 1, 2, 5, 5};
    int *output = new int[30];
    int *cumsum = new int[30];
    cumsum[0] = data[0];
    for (int i = 1; i < 24; i++) {
        cumsum[i] = data[i] + cumsum[i - 1];
    }
    encode(data, output, 24, entrySize);

    SimdDeltaScanner32 *scanner = new SimdDeltaScanner32(entrySize);

    uint8_t *result = new uint8_t[10];
    scanner->scan(output, 24, (int *) result, new Predicate(opr_eq, cumsum[3], 0));

    for (int i = 0; i < 24; i++) {
        int bitidx = (i % 8) >= 4 ? (11 - i % 8) : (3 - i % 8);
        if (i != 3)
            EXPECT_EQ(0, result[i / 8] & (1 << bitidx)) << i;
        else
            EXPECT_EQ(1 << bitidx, (result[i / 8] & (1 << bitidx))) << i;
    }

    delete[] cumsum;
    delete[] result;
}

TEST(SimdDeltaScanner32, less) {
    int entrySize = 29;
    int data[24] = {12, 3, 7, 7, 1, 4, 6, 2, 5, 3, 4,
                    4, 2, 1, 3, 4, 2, 1, 4, 5, 1, 2, 5, 5};
    int *output = new int[30];
    int *cumsum = new int[30];
    cumsum[0] = data[0];
    for (int i = 1; i < 24; i++) {
        cumsum[i] = data[i] + cumsum[i - 1];
    }
    encode(data, output, 24, entrySize);

    SimdDeltaScanner32 *scanner = new SimdDeltaScanner32(entrySize);

    uint8_t *result = new uint8_t[10];
    scanner->scan(output, 24, (int *) result, new Predicate(opr_less, 30, 0));

    for (int i = 0; i < 24; i++) {
        int bitidx = (i % 8) >= 4 ? (11 - i % 8) : (3 - i % 8);
        if (cumsum[i] < 30)
            EXPECT_EQ(1 << bitidx, result[i / 8] & (1 << bitidx)) << i;
        else
            EXPECT_EQ(0, (result[i / 8] & (1 << bitidx))) << i;
    }

    delete[] cumsum;
    delete[] result;
}

TEST(SimdDeltaScanner32, cumsum) {

    __m256i input = _mm256_setr_epi32(2,3,4,5,6,7,8,9);

    SimdDeltaScanner32 *scanner = new SimdDeltaScanner32(29);

    __m256i cumsum = scanner->cumsum(input);

    EXPECT_EQ(_mm256_extract_epi32(cumsum,0), 2);
    EXPECT_EQ(_mm256_extract_epi32(cumsum,1), 5);
    EXPECT_EQ(_mm256_extract_epi32(cumsum,2), 9);
    EXPECT_EQ(_mm256_extract_epi32(cumsum,3), 14);
    EXPECT_EQ(_mm256_extract_epi32(cumsum,4), 20);
    EXPECT_EQ(_mm256_extract_epi32(cumsum,5), 27);
    EXPECT_EQ(_mm256_extract_epi32(cumsum,6), 35);
    EXPECT_EQ(_mm256_extract_epi32(cumsum,7), 44);
}

