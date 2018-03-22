//
// Created by harper on 3/12/18.
//
#include <random>
#include <iostream>
#include <sys/time.h>
#include "scan/delta/SimdDeltaScanner32.h"
#include "scan/delta/SimdDeltaScanner16.h"
#include "scan/delta/TrivialDeltaScanner.h"
#include "util/encode.h"
#include "scan/delta/ParallelDeltaScanner.h"

uint64_t delta_throughput(Scanner *scanner, int es, uint64_t num, int *input, int *output, int *encoded) {
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist; // distribution in range [1, 6]

    for (int i = 0; i < num; i++) {
        input[i] = dist(rng);
    }
    // Large enough
    encode(input, encoded, num, es);


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


    return num / elapse;
}


int main(int argc, char **argv) {
    uint64_t num = 100000000;

    int *input = (int *) aligned_alloc(64, sizeof(int) * num);
    int *encoded = (int *) aligned_alloc(64, sizeof(int) * num);
    int *output = (int *) aligned_alloc(64, sizeof(int) * num);


    int MAX_REPEAT = 5;
    for (int es = 3; es <= 32; es++) {
        for (int nti = 0; nti < 7; nti++) {
            int numThread = 1 << nti;
            uint64_t simd = 0L;
            Scanner *deltaScanner = new ParallelDeltaScanner(numThread, es);
            for (int repeat = 0; repeat < MAX_REPEAT; repeat++) {
                simd += delta_throughput(deltaScanner, es, num, input, output, encoded);
            }
            std::cout << es << "," << numThread << "," << simd / MAX_REPEAT << "," << std::endl;
        }
    }


    free(input);
    free(output);
    free(encoded);
}