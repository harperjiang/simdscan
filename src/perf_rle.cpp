//
// Created by harper on 3/12/18.
//
#include <random>
#include <iostream>
#include <sys/time.h>
#include "scan/Scanner.h"
#include "util/encode.h"
#include "scan/rle/TrivialRLEScanner.h"
#include "scan/rle/SimdRLEScanner.h"

int rle_throughput(Scanner *scanner, uint64_t num, int es, int rls) {
    int *input = (int *) aligned_alloc(64, sizeof(int) * num);

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist; // distribution in range [1, 6]

    for (int i = 0; i < num; i++) {
        input[i] = dist(rng);
    }
    // Large enough
    int *encoded = (int *) aligned_alloc(64, sizeof(int) * num);
    int numRle = encode_rle(input, encoded, num, es, rls);

    int *output = (int *) aligned_alloc(64, sizeof(int) * num);

    auto x = dist(rng);
//    Predicate p(opr_in, x / 2, x);
    Predicate p(opr_eq, x, 0);

    struct timeval tp;

    gettimeofday(&tp, NULL);
    long start, elapse;
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    scanner->scan(input, numRle, output, &p);

    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;

    printf("%d\n", output[232]);

    free(input);
    free(output);

    return num / elapse;
}

int main(int argc, char **argv) {
    uint64_t repeat = 100000000;

    for (int es = 5; es < 32; es++) {
        for (int rls = 5; rls < 32; rls++) {
            int trivial = rle_throughput(new TrivialRLEScanner(es, rls), repeat, es, rls);
            int aligned = rle_throughput(new SimdRLEScanner(es, rls, true), repeat, es, rls);
            int ualigned = rle_throughput(new SimdRLEScanner(es, rls, false), repeat, es, rls);
            std::cout << es << "," << rls << "," << (double) aligned / trivial << ","
                      << (double) ualigned / trivial << "," << trivial << "," << aligned << "," << ualigned
                      << std::endl;
        }
    }
}