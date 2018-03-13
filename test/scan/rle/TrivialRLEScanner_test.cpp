//
// Created by harper on 3/12/18.
//


#include <gtest/gtest.h>
#include "../../../src/util/encode.h"
#include "../../../src/predicate/Predicate.h"
#include "../../../src/scan/rle/TrivialRLEScanner.h"

TEST(TrivialRLEScanner, TestScan) {
    int entrySize = 9;
    int rlSize = 2;
    int data[] = {2, 2, 3, 7, 7, 8, 9, 1, 2, 2, 4, 5, 29, 11,
                  6, 8, 22, 12, 12, 21, 21, 21, 4, 4, 4, 5, 5, 5};

    int *encoded = (int *) aligned_alloc(32, 100 * sizeof(int));
    int *output = (int *) aligned_alloc(32, 100 * sizeof(int));
    encode_rle(data, encoded, 20, entrySize, rlSize);

    TrivialRLEScanner *scanner = new TrivialRLEScanner(entrySize, rlSize);

    Predicate p(opr_less, 5, 0);

    scanner->scan(encoded, 17, output, &p);

    // 1,2,1,1,0,2,0,1,0,1,1,1,2,2,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,2,0,3,1,3,0,3

    int expected[] = {1, 2, 1, 1, 0, 2, 0, 1, 0, 1, 1, 1, 2, 2, 1, 1, 0, 1,
                      0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 0, 3, 1, 3, 0, 3};

    for (int i = 0; i < 34; i++) {
        EXPECT_EQ(expected[i], output[i]) << i;
    }
    free(encoded);
    free(output);
}