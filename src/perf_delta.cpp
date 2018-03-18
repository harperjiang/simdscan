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

int delta_throughput(Scanner *scanner, int es, uint64_t num) {
    int *input = (int *) aligned_alloc(64, sizeof(int) * num);

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist; // distribution in range [1, 6]

    for (int i = 0; i < num; i++) {
        input[i] = dist(rng);
    }
    // Large enough
    int *encoded = (int *) aligned_alloc(64, sizeof(int) * num);

    encode(input, encoded, num, es);

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

    free(input);
    free(output);

    return num / elapse;
}


int main(int argc, char **argv) {
    uint64_t repeat = 100000000;

    for (int es = 3; es < 31; es++) {
        int trivial = delta_throughput(new TrivialDeltaScanner(es), es, repeat);
        Scanner *deltaScanner;
        if (es <= 16) {
            deltaScanner = new SimdDeltaScanner16(es);
        } else {
            deltaScanner = new SimdDeltaScanner32(es);
        }
        int delta = delta_throughput(deltaScanner, es, repeat);
        std::cout << trivial << "," << delta << "," << delta / trivial << std::endl;
    }
}