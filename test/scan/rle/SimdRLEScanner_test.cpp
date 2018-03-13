//
// Created by harper on 3/13/18.
//
#include <gtest/gtest.h>
#include "../../../src/util/encode.h"
#include "../../../src/predicate/Predicate.h"
#include "../../../src/scan/rle/TrivialRLEScanner.h"
#include "../../../src/scan/rle/SimdRLEScanner.h"

TEST(SimdRLEScanner, TestLessAligned) {
    int entrySize = 9;
    int rlSize = 2;
    int data[] = {2, 2, 3, 7, 7, 8, 9, 1, 2, 2, 4, 5, 29, 11,
                  6, 8, 22, 12, 12, 21, 21, 21, 4, 4, 4, 5, 5, 5};

    int *encoded = (int *) aligned_alloc(64, 100 * sizeof(int));
    int *output = (int *) aligned_alloc(64, 100 * sizeof(int));
    encode_rle(data, encoded, 28, entrySize, rlSize);

    SimdRLEScanner *scanner = new SimdRLEScanner(entrySize, rlSize, true);

    Predicate p(opr_less, 5, 0);

    scanner->scan(encoded, 17, output, &p);

    int expected[] = {1, 2, 1, 1, 0, 2, 0, 1, 0, 1, 1, 1, 1, 2, 1, 1, 0, 1,
                      0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 0, 3, 1, 3, 0, 3};

    for (int i = 0; i < 17; i++) {
        int bitoff = i * 11 + 8;
        int intidx = bitoff / 32;
        int intoff = bitoff % 32;
        if (expected[i * 2]) {
            EXPECT_FALSE(output[intidx] & (1 << intoff)) << "Compare " << i;
        } else {
            EXPECT_TRUE(output[intidx] & (1 << intoff)) << "Compare " << i;
        }
        int extractedRl = extract_entry(output, (bitoff + 9) / 32, (bitoff + 9) % 32, 2);
        EXPECT_EQ(expected[i * 2 + 1], extractedRl) << "Rl " << i;
    }

    free(encoded);
    free(output);
}

TEST(SimdRLEScanner, TestLessUnaligned) {
    int entrySize = 9;
    int rlSize = 2;
    int data[] = {2, 2, 3, 7, 7, 8, 9, 1, 2, 2, 4, 5, 29, 11,
                  6, 8, 22, 12, 12, 21, 21, 21, 4, 4, 4, 5, 5, 5};

    int *encoded = (int *) aligned_alloc(64, 100 * sizeof(int));
    int *output = (int *) aligned_alloc(64, 100 * sizeof(int));
    encode_rle(data, encoded, 28, entrySize, rlSize);

    SimdRLEScanner *scanner = new SimdRLEScanner(entrySize, rlSize, false);

    Predicate p(opr_less, 5, 0);

    scanner->scan(encoded, 17, output, &p);

    int expected[] = {1, 2, 1, 1, 0, 2, 0, 1, 0, 1, 1, 1, 1, 2, 1, 1, 0, 1,
                      0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 0, 3, 1, 3, 0, 3};

    for (int i = 0; i < 17; i++) {
        int bitoff = i * 11 + 8;
        int intidx = bitoff / 32;
        int intoff = bitoff % 32;
        if (expected[i * 2]) {
            EXPECT_FALSE(output[intidx] & (1 << intoff)) << "Compare " << i;
        } else {
            EXPECT_TRUE(output[intidx] & (1 << intoff)) << "Compare " << i;
        }
        int extractedRl = extract_entry(output, (bitoff + 9) / 32, (bitoff + 9) % 32, 2);
        EXPECT_EQ(expected[i * 2 + 1], extractedRl) << "Rl " << i;
    }

    free(encoded);
    free(output);
}


TEST(SimdRLEScanner, TestEqAligned) {
    int entrySize = 9;
    int rlSize = 2;
    int data[] = {2, 2, 3, 7, 7, 8, 9, 1, 2, 2, 4, 5, 29, 11,
                  6, 8, 22, 12, 12, 21, 21, 21, 4, 4, 4, 5, 5, 5};

    int *encoded = (int *) aligned_alloc(64, 100 * sizeof(int));
    int *output = (int *) aligned_alloc(64, 100 * sizeof(int));
    encode_rle(data, encoded, 28, entrySize, rlSize);

    SimdRLEScanner *scanner = new SimdRLEScanner(entrySize, rlSize, true);

    Predicate p(opr_eq, 5, 0);

    scanner->scan(encoded, 17, output, &p);

    int expected[] = {0, 2, 0, 1, 0, 2, 0, 1, 0, 1, 0, 1, 0, 2, 0, 1, 1, 1,
                      0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 0, 3, 1, 3, 1, 3};

    for (int i = 0; i < 17; i++) {
        int bitoff = i * 11 + 8;
        int intidx = bitoff / 32;
        int intoff = bitoff % 32;
        if (expected[i * 2]) {
            EXPECT_FALSE(output[intidx] & (1 << intoff)) << "Compare " << i;
        } else {
            EXPECT_TRUE(output[intidx] & (1 << intoff)) << "Compare " << i;
        }
        int extractedRl = extract_entry(output, (bitoff + 9) / 32, (bitoff + 9) % 32, 2);
        EXPECT_EQ(expected[i * 2 + 1], extractedRl) << "Rl " << i;
    }

    free(encoded);
    free(output);
}

TEST(SimdRLEScanner, TestEqUnaligned) {
    int entrySize = 9;
    int rlSize = 2;
    int data[] = {2, 2, 3, 7, 7, 8, 9, 1, 2, 2, 4, 5, 29, 11,
                  6, 8, 22, 12, 12, 21, 21, 21, 4, 4, 4, 5, 5, 5};

    int *encoded = (int *) aligned_alloc(64, 100 * sizeof(int));
    int *output = (int *) aligned_alloc(64, 100 * sizeof(int));
    encode_rle(data, encoded, 28, entrySize, rlSize);

    SimdRLEScanner *scanner = new SimdRLEScanner(entrySize, rlSize, false);

    Predicate p(opr_eq, 5, 0);

    scanner->scan(encoded, 17, output, &p);

    int expected[] = {0, 2, 0, 1, 0, 2, 0, 1, 0, 1, 0, 1, 0, 2, 0, 1, 1, 1,
                      0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 0, 3, 1, 3, 1, 3};

    for (int i = 0; i < 17; i++) {
        int bitoff = i * 11 + 8;
        int intidx = bitoff / 32;
        int intoff = bitoff % 32;
        if (expected[i * 2]) {
            EXPECT_FALSE(output[intidx] & (1 << intoff)) << "Compare " << i;
        } else {
            EXPECT_TRUE(output[intidx] & (1 << intoff)) << "Compare " << i;
        }
        int extractedRl = extract_entry(output, (bitoff + 9) / 32, (bitoff + 9) % 32, 2);
        EXPECT_EQ(expected[i * 2 + 1], extractedRl) << "Rl " << i;
    }

    free(encoded);
    free(output);
}