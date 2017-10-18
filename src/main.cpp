#include <immintrin.h>
#include <random>
#include <iostream>
#include <stdint.h>
#include <sys/time.h>
#include "scan/Scanner.h"
#include "util/encode.h"
#include "scan/bitpack/WillhalmScanner128.h"
#include "scan/bitpack/HaoScanner128.h"
#include "scan/bitpack/HaoScanner256.h"
#include "scan/bitpack/HaoScanner512.h"
#include "scan/bitpack/BitWeaverHScanner128.h"
#include "scan/bitpack/BitWeaverHScanner256.h"
#include "scan/bitpack/BitWeaverHScanner512.h"
#include "scan/delta/TrivialDeltaScanner.h"
#include "scan/delta/SimdDeltaScanner128.h"
#include "scan/delta/SimdDeltaScanner256.h"

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


#pragma GCC push_options
#pragma GCC optimize ("O0")

int throughput(Scanner *scanner, uint64_t num, int entrySize) {
    int *input = new int[num];
    // Prepare random numbers
    int max = (1 << entrySize) - 1;

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, max); // distribution in range [1, 6]

    for (int i = 0; i < num; i++) {
        input[i] = dist(rng);
    }

    uint64_t encodedSize = ((num * entrySize / 512) + 1) * 16;
    int *encoded = (int *) aligned_alloc(64, sizeof(int) * encodedSize);

    encode(input, encoded, num, entrySize);

    // Large enough
    int *output = (int *) aligned_alloc(64, sizeof(int) * num);

    auto x = dist(rng);
//    Predicate p(opr_in, x / 2, x);
    Predicate p(opr_eq, x, 0);

    struct timeval tp;

    gettimeofday(&tp, NULL);
    long start, elapse;
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    scanner->scan(encoded, num, output, &p);

    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;

    delete[] input;
    free(encoded);
    free(output);

    return num / elapse;
}

int bwh_throughput(Scanner *scanner, uint64_t num, int entrySize) {
    int *input = new int[num];
    // Prepare random numbers
    int max = (1 << entrySize) - 1;

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, max); // distribution in range [1, 6]

    for (int i = 0; i < num; i++) {
        input[i] = dist(rng);
    }

    uint64_t numEntryInWord = 64 / (entrySize + 1);
    uint64_t numEntryInSimd = 8 * numEntryInWord;
    uint64_t numSimd = num / numEntryInSimd + (num % numEntryInSimd ? 1 : 0);

    uint64_t encodedSize = numSimd * 16;
    int *encoded = (int *) aligned_alloc(64, sizeof(int) * encodedSize);

    bitweaverh_encode(input, encoded, num, entrySize);

    // Large enough
    int *output = (int *) aligned_alloc(64, sizeof(int) * num);

    auto x = dist(rng);
//    Predicate p(opr_in, x / 2, x);
    Predicate p(opr_eq, x, 0);

    struct timeval tp;

    gettimeofday(&tp, NULL);
    long start, elapse;
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    scanner->scan(encoded, num, output, &p);

    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;

    delete[] input;
    free(encoded);
    free(output);

    return num / elapse;
}

int delta_throughput(Scanner *scanner, uint64_t num) {
    int *input = (int *) aligned_alloc(64, sizeof(int) * num);

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist; // distribution in range [1, 6]

    for (int i = 0; i < num; i++) {
        input[i] = dist(rng);
    }
    // Large enough
    int *output = (int *) aligned_alloc(64, sizeof(int) * num);

    auto x = dist(rng);
//    Predicate p(opr_in, x / 2, x);
    Predicate p(opr_eq, x, 0);

    struct timeval tp;

    gettimeofday(&tp, NULL);
    long start, elapse;
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    scanner->scan(input, num, output, &p);

    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;

    free(input);
    free(output);

    return num / elapse;
}

#pragma GCC pop_options

int main(int argc, char **argv) {
    uint64_t repeat = 100000000;

    int tds = delta_throughput(new TrivialDeltaScanner(true), repeat);
    int tdn = delta_throughput(new TrivialDeltaScanner(false), repeat);
    int sds128 = delta_throughput(new SimdDeltaScanner128(true), repeat);
    int sdn128 = delta_throughput(new SimdDeltaScanner128(false), repeat);
    int sds256 = delta_throughput(new SimdDeltaScanner256(true), repeat);
    int sdn256 = delta_throughput(new SimdDeltaScanner256(false), repeat);

    std::cout << (double) sds128 / tds << "," << (double) sdn128 / tdn << "," << (double) sds256 / tds << ","
              << (double) sdn256 / tdn << std::endl;
//    for (int es = 5; es < 30; es++) {
//        int h128 = throughput(new HaoScanner128(es, true), repeat, es);
//        int h256 = throughput(new HaoScanner256(es, true), repeat, es);
//        int h512 = throughput(new HaoScanner512(es, true), repeat, es);
//        int bwh128 = bwh_throughput(new BitWeaverHScanner128(es), repeat, es);
//        int bwh256 = bwh_throughput(new BitWeaverHScanner256(es), repeat, es);
//        int bwh512 = bwh_throughput(new BitWeaverHScanner512(es), repeat, es);
//        int w = throughput(new WillhalmScanner128(es, true), repeat, es);
//        std::cout << es << "," << ((double) h128 / bwh128) << "," << ((double) h256 / bwh256) << ","
//                  << ((double) h512 / bwh512)
//                  << std::endl;
//    }
}

