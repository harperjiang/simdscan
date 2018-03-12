#include <immintrin.h>
#include <random>
#include <iostream>
#include <stdint.h>
#include <sys/time.h>

//
// Created by harper on 3/12/18.
//
void ustore() {

}

void store() {
    struct timeval tp;

    gettimeofday(&tp, NULL);
    printf("Starting %ld\n", tp.tv_sec * 1000 + tp.tv_usec / 1000);
    int *dest = new int[10000000];
    for (int i = 0; i < 2500000; i++) {
        __m128i data = _mm_set1_epi32(5);
        _mm_stream_si128((__m128i *) (dest + i * 4), data);
    }
//	for (int i = 0; i < 10000000; i++) {
//		dest[i] = 5;
//	}

    gettimeofday(&tp, NULL);
    printf("Stopping %ld\n", tp.tv_sec * 1000 + tp.tv_usec / 1000);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void uloadSpeed() {

    __m256i result = _mm256_set1_epi32(8);

    struct timeval start;
    struct timeval stop;

    gettimeofday(&start, NULL);
    // Aligned Load
    int *aligned = (int *) aligned_alloc(2048, 8000000 * sizeof(int));
    for (int i = 0; i < 1000000; i++) {
        __m256i a = _mm256_loadu_si256((__m256i *) (aligned + i * 8));
        result = _mm256_xor_si256(a, result);
    }
    gettimeofday(&stop, NULL);
    printf("Time for Aligned ULoad: %ld\n", stop.tv_sec * 1000 + stop.tv_usec / 1000
                                            - start.tv_sec * 1000 - start.tv_usec / 1000);

    printf("%d\n", _mm256_extract_epi64(result, 0));
    printf("%d\n", _mm256_extract_epi64(result, 1));
    printf("%d\n", _mm256_extract_epi64(result, 2));
    printf("%d\n", _mm256_extract_epi64(result, 3));

    // Unaligned load
    gettimeofday(&start, NULL);
    int *unaligned = new int[1000007];
    for (int i = 0; i < 1000000; i++) {
        __m256i a = _mm256_loadu_si256((__m256i *) (unaligned + i));
        result = _mm256_xor_si256(a, result);
    }
    gettimeofday(&stop, NULL);
    printf("Time for Unaligned Load: %ld\n", stop.tv_sec * 1000 + stop.tv_usec / 1000
                                             - start.tv_sec * 1000 - start.tv_usec / 1000);

    printf("%d\n", _mm256_extract_epi64(result, 0));
    printf("%d\n", _mm256_extract_epi64(result, 1));
    printf("%d\n", _mm256_extract_epi64(result, 2));
    printf("%d\n", _mm256_extract_epi64(result, 3));

    // Aligned Load
    gettimeofday(&start, NULL);
    for (int i = 0; i < 1000000; i++) {
        __m256i a = _mm256_stream_load_si256((__m256i *) (aligned + i * 8));
        result = _mm256_xor_si256(a, result);
    }
    gettimeofday(&stop, NULL);
    printf("Time for aligned Load: %ld\n", stop.tv_sec * 1000 + stop.tv_usec / 1000
                                           - start.tv_sec * 1000 - start.tv_usec / 1000);

    printf("%d\n", _mm256_extract_epi64(result, 0));
    printf("%d\n", _mm256_extract_epi64(result, 1));
    printf("%d\n", _mm256_extract_epi64(result, 2));
    printf("%d\n", _mm256_extract_epi64(result, 3));
}

#pragma GCC pop_options
void load() {
    int data[] = {2, 4, 8, 16, 0};
    void *d = data;
    __m128i x0 = _mm_loadu_si128((__m128i *) d);
    __m128i x = _mm_loadu_si128((__m128i *) (d + 1));

    printf("%d,%d,%d,%d", _mm_extract_epi32(x, 0), _mm_extract_epi32(x, 1),
           _mm_extract_epi32(x, 2), _mm_extract_epi32(x, 3));
}

int main(int argc, char **argv) {
    uloadSpeed();
}
