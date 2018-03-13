//
// Created by harper on 3/12/18.
//


#include <gtest/gtest.h>

TEST(TrivialRLEScanner, TestScan) {
int entrySize = 9;
int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
              18, 19, 20};

int *encoded = (int *) aligned_alloc(32, 32 * sizeof(int));
int *output = (int *) aligned_alloc(32, 32 * sizeof(int));
encode(data, encoded, 20, entrySize);

HaoScanner256 *scanner = new HaoScanner256(entrySize, true);

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