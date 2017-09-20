/*
 * template.h
 *
 *  Created on: Sep 5, 2017
 *      Author: harper
 */

#ifndef SRC_UTIL_TEMPLATE_H_
#define SRC_UTIL_TEMPLATE_H_

#include <immintrin.h>

int mm_extract_epi32(__m128i data, int index);
__m128i mm_alignr_epi8(__m128i a, __m128i b, int offset);
__m128d mm_blend_pd(__m128d a, __m128d b, int imm);

int mm256_extract_epi32(__m256i data, int index);
__m256d mm256_blend_pd(__m256d a, __m256d b, int imm);
__m256i mm256_alignr_epi8(__m256i a, __m256i b, int imm);

__m512i mm512_blend_epi64(__m512i a, __m512i b, int imm);
#endif /* SRC_UTIL_TEMPLATE_H_ */
