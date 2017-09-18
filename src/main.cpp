#include <immintrin.h>
#include <random>
#include <iostream>
#include <sys/time.h>
#include "scan/Scanner.h"
#include "util/encode.h"
#include "scan/WillhalmScanner128.h"
#include "scan/HaoScanner128.h"
#include "scan/HaoScanner.h"

void storeSpeed() {
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

void uloadSpeed() {

    __m256i result = _mm256_set1_epi32(8);

    struct timeval tp;

    gettimeofday(&tp, NULL);
    printf("Starting %ld\n", tp.tv_sec * 1000 + tp.tv_usec / 1000);
    // Aligned Load
    int *aligned = (int *) aligned_alloc(2048, 8000000 * sizeof(int));
    for (int i = 0; i < 1000000; i++) {
        __m256i a = _mm256_load_si256((__m256i *) (aligned + i * 8));
        result = _mm256_xor_si256(a, result);
    }
    gettimeofday(&tp, NULL);
    printf("Stopping %ld\n", tp.tv_sec * 1000 + tp.tv_usec / 1000);
    // Unaligned load
    int *unaligned = new int[1000007];
    for (int i = 0; i < 1000000; i++) {
        __m256i a = _mm256_loadu_si256((__m256i *) (unaligned + i));
        result = _mm256_xor_si256(a, result);
    }
    gettimeofday(&tp, NULL);
    printf("Stopping %ld\n", tp.tv_sec * 1000 + tp.tv_usec / 1000);

    for (int i = 0; i < 1000000; i++) {
        __m256i a = _mm256_stream_load_si256((__m256i *) (aligned + i * 8));
        result = _mm256_xor_si256(a, result);
    }
    gettimeofday(&tp, NULL);
    printf("Stopping %ld\n", tp.tv_sec * 1000 + tp.tv_usec / 1000);

    printf("%d\n", _mm256_extract_epi32(result, 0));
}

void loadalign() {
    int data[] = {2, 4, 8, 16, 0};
    void *d = data;
    __m128i x0 = _mm_loadu_si128((__m128i *) d);
    __m128i x = _mm_loadu_si128((__m128i *) (d + 1));

    printf("%d,%d,%d,%d", _mm_extract_epi32(x, 0), _mm_extract_epi32(x, 1),
           _mm_extract_epi32(x, 2), _mm_extract_epi32(x, 3));
}

int throughput(Scanner *scanner, int num, int entrySize) {
    int *input = new int[num];
    // Prepare random numbers
    int max = (1 << entrySize) - 1;

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, max); // distribution in range [1, 6]

    for (int i = 0; i < num; i++) {
        input[i] = dist(rng);
    }

    int encodedSize = ((num * entrySize / 256) + 1) * 8;
    int *encoded = (int *) aligned_alloc(32, sizeof(int) * encodedSize);

    encode(input, encoded, num, entrySize);

    // Large enough
    int *output = (int *) aligned_alloc(32, sizeof(int) * num);

    auto x = dist(rng);
//    Predicate p(opr_in, x / 2, x);
    Predicate p(opr_eq, x, 0);

    struct timeval tp;

    gettimeofday(&tp, NULL);
    long start, elapse;
    start = tp.tv_sec * 1000000 + tp.tv_usec;

    scanner->scan(encoded, num, output, &p);

    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000000 + tp.tv_usec - start;

    delete[] input;
    delete[] encoded;
    delete[] output;

    return num / elapse;
}

int main(int argc, char **argv) {
    int repeat = 10000000;
    for (int es = 5; es < 30; es++) {
        int hs = throughput(new HaoScanner128(es, true), repeat, es);
        int hs256 = throughput(new HaoScanner(es), repeat, es);
        int ws = throughput(new WillhalmScanner128(es, true), repeat, es);
        std::cout << es << "," << ((double) hs / ws) << "," << ((double) hs256 / ws) << "," << ws << std::endl;
    }
}

