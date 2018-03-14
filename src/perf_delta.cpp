//
// Created by harper on 3/12/18.
//
#include <random>
#include <iostream>
#include <sys/time.h>
#include "scan/delta/SimdDeltaScanner32.h"
#include "scan/delta/SimdDeltaScanner16.h"
#include "scan/delta/TrivialDeltaScanner.h"

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

    printf("%d\n", output[3234]);

    free(input);
    free(output);

    return num / elapse;
}


int main(int argc, char **argv) {
    uint64_t repeat = 100000000;

    int tds = delta_throughput(new TrivialDeltaScanner(true), repeat);
    int tdn = delta_throughput(new TrivialDeltaScanner(false), repeat);
    int sds128 = delta_throughput(new SimdDeltaScanner32(true), repeat);
    int sdn128 = delta_throughput(new SimdDeltaScanner32(false), repeat);
    int sds256 = delta_throughput(new SimdDeltaScanner16(true), repeat);
    int sdn256 = delta_throughput(new SimdDeltaScanner16(false), repeat);

    std::cout << tds << "," << tdn << "," << sds128 << "," << sdn128 << "," << sds256 << "," << sdn256 << std::endl;

}