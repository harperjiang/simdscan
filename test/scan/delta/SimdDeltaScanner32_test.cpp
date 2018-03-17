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
        if (i != 3)
            EXPECT_EQ(0, result[i / 8] & (1 << (7 - i % 8))) << i;
        else
            EXPECT_EQ(1 << (7 - i % 8), (result[i / 8] & (1 << (7 - i % 8)))) << i;
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
        if (cumsum[i] < 30)
            EXPECT_EQ(1 << (7 - i % 8), result[i / 8] & (1 << (7 - i % 8))) << i;
        else
            EXPECT_EQ(0, (result[i / 8] & (1 << (7 - i % 8)))) << i;
    }

    delete[] cumsum;
    delete[] result;
}

