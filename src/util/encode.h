/*
 * encode.h
 *
 *  Created on: Sep 9, 2017
 *      Author: harper
 */

#ifndef SRC_UTIL_ENCODE_H_
#define SRC_UTIL_ENCODE_H_

#include <immintrin.h>

/**
 * Bit-packed Encode given data
 * @param input
 * @param output
 * @param length
 * @param entrySize
 */
void encode(int *input, int *output, int length, int entrySize);

/**
 * Bit-packed used by BitWeaving
 * @param incode
 * @param output
 * @param length
 * @param entrySize
 */
void bitweaverh_encode(int *incode, int *output, int length, int entrySize);

/**
 * RLE-BP given data
 * @param input
 * @param output
 * @param length
 * @param entrySize
 * @param rlSize
 * @return
 */
int encode_rle(int *input, int *output, int length, int entrySize, int rlSize);

/**
 * Extract data from a bit-packed stream
 * @param input
 * @param index
 * @param offset
 * @param entrySize
 * @return
 */
int extract_entry(int *input, int index, int offset, int entrySize);

/**
 * Unpack bit-packed entries in the given location to SIMD buffer as 16bit int
 *
 * This is for entries less than 8 bit
 */
__m256i unpack16_small(uint8_t *input, int offset, int entrySize);

/**
 * Unpack bit-packed entries in the given location to SIMD buffer as 16bit int
 *
 *  For entries between 8 and 16 bits
 */
__m256i unpack16_large(uint8_t *input, int offset, int entrySize);


#endif /* SRC_UTIL_ENCODE_H_ */
