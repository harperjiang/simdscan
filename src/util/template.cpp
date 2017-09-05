/*
 * template.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: harper
 */

#include "template.h"

int mm_extract_epi32(__m128i data, int index) {
	switch (index) {
	case 0:
		return _mm_extract_epi32(data, 0);
	case 1:
		return _mm_extract_epi32(data, 1);
	case 2:
		return _mm_extract_epi32(data, 2);
	case 3:
	default:
		return _mm_extract_epi32(data, 3);
	}
}

__m128i mm_align_epi8(__m128i a, __m128i b, int offset) {
	switch (offset) {
	case 0:
		return b;
	case 1:
		return _mm_alignr_epi8(a, b, 1);
	case 2:
		return _mm_alignr_epi8(a, b, 2);
	case 3:
		return _mm_alignr_epi8(a, b, 3);
	case 4:
		return _mm_alignr_epi8(a, b, 4);
	case 5:
		return _mm_alignr_epi8(a, b, 5);
	case 6:
		return _mm_alignr_epi8(a, b, 6);
	case 7:
		return _mm_alignr_epi8(a, b, 7);
	case 8:
		return _mm_alignr_epi8(a, b, 8);
	case 9:
		return _mm_alignr_epi8(a, b, 9);
	case 10:
		return _mm_alignr_epi8(a, b, 10);
	case 11:
		return _mm_alignr_epi8(a, b, 11);
	case 12:
		return _mm_alignr_epi8(a, b, 12);
	case 13:
		return _mm_alignr_epi8(a, b, 13);
	case 14:
		return _mm_alignr_epi8(a, b, 14);
	case 15:
		return _mm_alignr_epi8(a, b, 15);
	default:
		return b;
	}
}

__m256d mm256_blend_pd(__m256d a, __m256d b, int imm) {
	switch (imm) {
	case 0:
		return _mm256_blend_pd(a, b, 0);
	case 1:
		return _mm256_blend_pd(a, b, 1);
	case 2:
		return _mm256_blend_pd(a, b, 2);
	case 3:
		return _mm256_blend_pd(a, b, 3);
	case 4:
		return _mm256_blend_pd(a, b, 4);
	case 5:
		return _mm256_blend_pd(a, b, 5);
	case 6:
		return _mm256_blend_pd(a, b, 6);
	case 7:
		return _mm256_blend_pd(a, b, 7);
	case 8:
		return _mm256_blend_pd(a, b, 8);
	case 9:
		return _mm256_blend_pd(a, b, 9);
	case 10:
		return _mm256_blend_pd(a, b, 10);
	case 11:
		return _mm256_blend_pd(a, b, 11);
	case 12:
		return _mm256_blend_pd(a, b, 12);
	case 13:
		return _mm256_blend_pd(a, b, 13);
	case 14:
		return _mm256_blend_pd(a, b, 14);
	case 15:
		return _mm256_blend_pd(a, b, 15);
	default:
		return _mm256_blend_pd(a, b, 0);
	}
}

