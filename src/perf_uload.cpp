#include <immintrin.h>
#include <random>
#include <iostream>
#include <stdint.h>
#include <sys/time.h>

//
// Created by harper on 3/12/18.
//

void uloadSpeed() {

    __m512i result = _mm512_set1_epi32(8);

    struct timeval start;
    struct timeval stop;

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist; // distribution in range [1, 6]

    uint64_t num = 1600000001;
    uint64_t ite = 100000000;
    int *input = (int *) aligned_alloc(64, num * sizeof(int));

    for (int i = 0; i < num; i++) {
        input[i] = dist(rng);
    }

    gettimeofday(&start, NULL);
    // Aligned Load
    __m512i *aligned = (__m512i *) input;

    for (int i = 0; i < ite; i++) {
        __m512i a = _mm512_stream_load_si512(aligned + i);
        result = _mm512_xor_si512(a, result);
    }
    gettimeofday(&stop, NULL);
    printf("Time for Aligned ULoad: %ld\n", stop.tv_sec * 1000 + stop.tv_usec / 1000
                                            - start.tv_sec * 1000 - start.tv_usec / 1000);

    printf("%d\n", _mm512_extracti64x4_epi64(result, 0));

    __m512i *unaligned = (__m512i *) (((void *) input) + 1);
    // Unaligned load
    gettimeofday(&start, NULL);
    for (int i = 0; i < ite; i++) {
        __m512i a = _mm512_loadu_si512(unaligned + i);
        result = _mm512_xor_si512(a, result);
    }
    gettimeofday(&stop, NULL);
    printf("Time for Unaligned Load: %ld\n", stop.tv_sec * 1000 + stop.tv_usec / 1000
                                             - start.tv_sec * 1000 - start.tv_usec / 1000);

    printf("%d\n", _mm512_extracti64x4_epi64(result, 0));

    // Aligned Load
    gettimeofday(&start, NULL);
    for (int i = 0; i < ite; i++) {
        __m512i a = _mm512_loadu_si512(aligned + i);
        result = _mm512_xor_si512(a, result);
    }
    gettimeofday(&stop, NULL);
    printf("Time for unaligned Load on aligned data: %ld\n", stop.tv_sec * 1000 + stop.tv_usec / 1000
                                                             - start.tv_sec * 1000 - start.tv_usec / 1000);

    printf("%d\n", _mm512_extracti64x4_epi64(result, 0));
}


int main(int argc, char **argv) {
    uloadSpeed();
}
