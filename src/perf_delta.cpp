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
#include "scan/delta/LemireDeltaScanner.h"

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
    for (int es = 5; es <= 32; es++) {
        uint64_t trivial = 0L;
        uint64_t simd = 0L;
        uint64_t lemire = 0L;
        for (int repeat = 0; repeat < MAX_REPEAT; repeat++) {
            trivial += delta_throughput(new TrivialDeltaScanner(es), es, num, input, output, encoded);
            lemire += delta_throughput(new LemireDeltaScanner(es), es, num, input, output, encoded);
            Scanner *deltaScanner;
            if (es <= 16) {
                deltaScanner = new SimdDeltaScanner16(es);
            } else {
                deltaScanner = new SimdDeltaScanner32(es);
            }
            simd += delta_throughput(deltaScanner, es, num, input, output, encoded);
        }
        std::cout << es << "," << trivial / MAX_REPEAT << "," << simd / MAX_REPEAT << "," << lemire / MAX_REPEAT
                  << std::endl;
    }


    free(input);
    free(output);
    free(encoded);
}