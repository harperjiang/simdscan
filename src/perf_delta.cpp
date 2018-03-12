//
// Created by harper on 3/12/18.
//
#include <random>
#include <iostream>
#include <sys/time.h>
#include "scan/delta/SimdDeltaScanner128.h"
#include "scan/delta/SimdDeltaScanner256.h"
#include "scan/delta/TrivialDeltaScanner.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

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

int main(int argc, char** argv) {
    uint64_t repeat = 100000000;

    int tds = delta_throughput(new TrivialDeltaScanner(true), repeat);
    int tdn = delta_throughput(new TrivialDeltaScanner(false), repeat);
    int sds128 = delta_throughput(new SimdDeltaScanner128(true), repeat);
    int sdn128 = delta_throughput(new SimdDeltaScanner128(false), repeat);
    int sds256 = delta_throughput(new SimdDeltaScanner256(true), repeat);
    int sdn256 = delta_throughput(new SimdDeltaScanner256(false), repeat);

    std::cout << (double) sds128 / tds << "," << (double) sdn128 / tdn << "," << (double) sds256 / tds << ","
              << (double) sdn256 / tdn << std::endl;

}