//
// Created by harper on 3/20/18.
//

#include <gtest/gtest.h>
#include "../../../src/util/encode.h"
#include "../../../src/scan/bitpack/WillhalmUnpackerScanner.h"

TEST(WillhalmScannerUnpacked, TestScan) {

    int entrySize = 9;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                  18, 19, 20};
    int result[8];
    int output[30];
    encode(data, result, 20, entrySize);

    WillhalmUnpackerScanner *scanner = new WillhalmUnpackerScanner(entrySize);

    Predicate p(opr_eq, 18, 0);

    scanner->scan(result, 20, output, &p);
    for (int i = 0; i < 20; i++) {
        int idx = i / 32;
        int off = i % 32;
        if (data[i] == 18)
            EXPECT_EQ(1 << off, output[idx] & (1 << off)) << i;
        else
            EXPECT_EQ(0, output[idx] & (1 << off)) << i;
    }

    Predicate p2(opr_less, 13, 7);

    scanner->scan(result, 20, output, &p2);
    for (int i = 0; i < 20; i++) {
        int idx = i / 32;
        int off = i % 32;
        if (data[i] < 13)
            EXPECT_EQ(1 << off, output[idx] & (1 << off)) << i;
        else
            EXPECT_EQ(0, output[idx] & (1 << off)) << i;
    }
}
