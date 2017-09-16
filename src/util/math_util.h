/*
 * math_util.h
 *
 *  Created on: Sep 6, 2017
 *      Author: harper
 */

#ifndef SRC_UTIL_MATH_UTIL_H_
#define SRC_UTIL_MATH_UTIL_H_

#include <immintrin.h>

__m128i mm_add_epi128(__m128i, __m128i);
__m128i mm_sub_epi128(__m128i, __m128i);

__m256i mm256_add_epi256(__m256i, __m256i);
__m256i mm256_sub_epi256(__m256i, __m256i);
#endif /* SRC_UTIL_MATH_UTIL_H_ */
