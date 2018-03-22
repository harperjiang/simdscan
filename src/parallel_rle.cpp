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
#include "scan/ParallelScanner.h"


uint64_t rle_throughput(Scanner *scanner, uint64_t num, int es, int rls, int *input, int *output, int *encoded) {
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist; // distribution in range [1, 6]

    for (int i = 0; i < num; i++) {
        input[i] = dist(rng);
    }
    // Large enough
    int numRle = encode_rle(input, encoded, num, es, rls);


    auto x = dist(rng);
//    Predicate p(opr_in, x / 2, x);
    Predicate p(opr_eq, x, 0);

    struct timeval tp;

    gettimeofday(&tp, NULL);
    long start, elapse;
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    scanner->scan(encoded, numRle, output, &p);

    gettimeofday(&tp, NULL);
    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;

    // printf("%d\n", output[232]);


    return num / elapse;
}

int main(int argc, char **argv) {
    uint64_t num = 100000000;
    int *input = (int *) aligned_alloc(64, sizeof(int) * num);
    int *encoded = (int *) aligned_alloc(64, sizeof(int) * 2 * num);
    int *output = (int *) aligned_alloc(64, sizeof(int) * 2 * num);

    int MAX_REPEAT = 5;
    for (int rls = 5; rls < 32; rls++) {
        for (int es = 5; es < 32; es++) {
            for (int nti = 0; nti < 7; nti++) {
                int numThread = 1 << nti;
                uint64_t time = 0;
                Scanner *scanner = new ParallelScanner(numThread, es + rls, new SimdRLEScanner(es, rls, false));
                for (int repeat = 0; repeat < MAX_REPEAT; repeat++) {
                    time += rle_throughput(scanner, num, es, rls, input, output, encoded);
                }
                std::cout << es << "," << rls << "," << numThread << "," << time / MAX_REPEAT << std::endl;
            }
        }
    }

    free(input);
    free(output);
    free(encoded);
}