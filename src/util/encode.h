/*
 * encode.h
 *
 *  Created on: Sep 9, 2017
 *      Author: harper
 */

#ifndef SRC_UTIL_ENCODE_H_
#define SRC_UTIL_ENCODE_H_

void encode(int* input, int* output, int length, int entrySize);

void bitweaverh_encode(int* incode, int* output, int length, int entrySize);

void encode_rle(int* input, int* output, int length, int entrySize, int rlSize);

#endif /* SRC_UTIL_ENCODE_H_ */
