#include <immintrin.h>
#include <random>
#include <iostream>
#include <stdint.h>
#include <sys/time.h>
#include <ostream>
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


int bp_throughput(Scanner *scanner, uint64_t num, int entrySize) {
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

    // Hopefully GCC will not optimize out invocation
    printf("%d\n", output[136]);

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

    printf("%d\n", output[136]);

    delete[] input;
    free(encoded);
    free(output);

    return num / elapse;
}

int main(int argc, char **argv) {
    uint64_t repeat = 100000000;

    for (int es = 5; es < 30; es++) {
        int h128 = bp_throughput(new HaoScanner128(es, true), repeat, es);
        int h256 = bp_throughput(new HaoScanner256(es, true), repeat, es);
        int h512 = bp_throughput(new HaoScanner512(es, true), repeat, es);
        int bwh128 = bwh_throughput(new BitWeaverHScanner128(es), repeat, es);
        int bwh256 = bwh_throughput(new BitWeaverHScanner256(es), repeat, es);
        int bwh512 = bwh_throughput(new BitWeaverHScanner512(es), repeat, es);
        int w = bp_throughput(new WillhalmScanner128(es, true), repeat, es);
        std::cout << es << "," << ((double) h128 / bwh128) << "," << ((double) h256 / bwh256) << ","
                  << ((double) h512 / bwh512) << "," << ((double) h512 / w)
                  << std::endl;
    }
}

