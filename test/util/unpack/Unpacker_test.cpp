//
// Created by harper on 3/15/18.
//

#include <gtest/gtest.h>
#include "../../../src/util/encode.h"
#include "../../../src/util/unpack/Small16Unpacker.h"
#include "../../../src/util/unpack/Large16Unpacker.h"

int mm256_extract_epi16(__m256i input, int index);


TEST(Small16Unpacker, unpack) {
    int entrySize = 7;
    int data[17] = {124, 31, 75, 77, 15, 41, 26, 22, 52, 32, 34,
                    41, 42, 14, 13, 14, 255};
    int output[16];
    encode(data, output, 17, entrySize);

    Small16Unpacker *unpacker = new Small16Unpacker(entrySize);


    for (int o = 0; o < 8; o++) {
        __m256i unpacked = unpacker->unpack((uint8_t *) output, o);

        for (int i = 0; i < 16; i++) {
            int bitoff = o + i * entrySize;
            int extract = extract_entry(output, bitoff / 8, bitoff % 8, entrySize);
            ASSERT_EQ(extract, mm256_extract_epi16(unpacked, i));
        }
    }
}


TEST(Large16Unpacker, unpack) {
    int entrySize = 15;
    int data[17] = {824, 331, 875, 1277, 3135, 2241, 26, 2192, 552, 532, 342,
                    411, 422, 1414, 13442, 4414, 32767};
    int output[16];
    encode(data, output, 17, entrySize);

    Large16Unpacker *unpacker = new Large16Unpacker(entrySize);


    for (int o = 0; o < 8; o++) {
        __m256i unpacked = unpacker->unpack((uint8_t *) output, o);

        for (int i = 0; i < 16; i++) {
            int bitoff = o + i * entrySize;
            int extract = extract_entry(output, bitoff / 8, bitoff % 8, entrySize);
            ASSERT_EQ(extract, mm256_extract_epi16(unpacked, i));
        }
    }
}

TEST(Small32Unpacker, unpack) {
    int entrySize = 21;
    int data[17] = {124, 31, 75, 77, 15, 41, 26, 22, 52, 32, 34,
                    41, 42, 14, 13, 14, 255};
    int output[16];
    encode(data, output, 17, entrySize);

    Small16Unpacker *unpacker = new Small16Unpacker(entrySize);


    for (int o = 0; o < 8; o++) {
        __m256i unpacked = unpacker->unpack((uint8_t *) output, o);

        for (int i = 0; i < 8; i++) {
            int bitoff = o + i * entrySize;
            int extract = extract_entry(output, bitoff / 8, bitoff % 8, entrySize);
            ASSERT_EQ(extract, mm256_extract_epi16(unpacked, i));
        }
    }
}

TEST(Large32Unpacker, unpack) {
    int entrySize = 30;
    int data[17] = {824, 331, 875, 1277, 3135, 2241, 26, 2192, 552, 532, 342,
                    411, 422, 1414, 13442, 4414, 32767};
    int output[16];
    encode(data, output, 17, entrySize);

    Large16Unpacker *unpacker = new Large16Unpacker(entrySize);


    for (int o = 0; o < 8; o++) {
        __m256i unpacked = unpacker->unpack((uint8_t *) output, o);

        for (int i = 0; i < 8; i++) {
            int bitoff = o + i * entrySize;
            int extract = extract_entry(output, bitoff / 8, bitoff % 8, entrySize);
            ASSERT_EQ(extract, mm256_extract_epi16(unpacked, i));
        }
    }
}

int mm256_extract_epi16(__m256i input, int index) {
    switch (index) {
        case 0:
            return _mm256_extract_epi16(input, 0);
        case 1:
            return _mm256_extract_epi16(input, 1);
        case 2:
            return _mm256_extract_epi16(input, 2);
        case 3:
            return _mm256_extract_epi16(input, 3);
        case 4:
            return _mm256_extract_epi16(input, 4);
        case 5:
            return _mm256_extract_epi16(input, 5);
        case 6:
            return _mm256_extract_epi16(input, 6);
        case 7:
            return _mm256_extract_epi16(input, 7);
        case 8:
            return _mm256_extract_epi16(input, 8);
        case 9:
            return _mm256_extract_epi16(input, 9);
        case 10:
            return _mm256_extract_epi16(input, 10);
        case 11:
            return _mm256_extract_epi16(input, 11);
        case 12:
            return _mm256_extract_epi16(input, 12);
        case 13:
            return _mm256_extract_epi16(input, 13);
        case 14:
            return _mm256_extract_epi16(input, 14);
        case 15:
            return _mm256_extract_epi16(input, 15);
        default:
            return 0;
    }
}