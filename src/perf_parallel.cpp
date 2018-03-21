#include <immintrin.h>
#include <random>
#include <iostream>
#include "scan/Scanner.h"
#include "util/encode.h"
#include "scan/bitpack/HaoLaneLoaderScanner.h"
#include "scan/ParallelScanner.h"

int bp_throughput(Scanner *scanner, uint64_t num, int entrySize, int *input, int *encoded, int *output) {
    // Prepare random numbers
    int max = (1 << entrySize) - 1;

    srand(time(NULL));

    for (int i = 0; i < num; i++) {
        input[i] = rand();
    }

    encode(input, encoded, num, entrySize);

    // Large enough

    int x = rand();
//    Predicate p(opr_in, x / 2, x);
    Predicate p(opr_eq, x, 0);

//    struct timeval tp;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);    /* mark start time */

//    gettimeofday(&tp, NULL);
//    long start, elapse;
//    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    scanner->scan(encoded, num, output, &p);

//    gettimeofday(&tp, NULL);
//    elapse = tp.tv_sec * 1000 + tp.tv_usec / 1000 - start;

    clock_gettime(CLOCK_MONOTONIC, &end);    /* mark the end time */
    uint64_t elapse = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_nsec - start.tv_nsec) / 1000L;

//    fprintf(stderr, "%d\n", output[123]);
    return (double) num / elapse;
}

//#pragma GCC pop_options
int main(int argc, char **argv) {
    uint64_t num = 500000000L;
    uint32_t entryChoice[] = {4, 9, 15, 22, 31};
    int *bp_input = new int[num];
    int *bp_encoded = (int *) aligned_alloc(64, sizeof(int) * (2 * num));
    int *bp_output = (int *) aligned_alloc(64, sizeof(int) * (2 * num));

    int MAX_REPEAT = 5;

    for (int ei = 0; ei < 5; ei++) {
        uint32_t es = entryChoice[ei];
        for (int thread = 0; thread < 5; thread++) {
            int numThread = 1 << thread;
            uint32_t fast = 0;
            Scanner *scanner = new ParallelScanner(numThread, es, new HaoLaneLoaderScanner(es));
            for (int repeat = 0; repeat < MAX_REPEAT; repeat++) {
                fast += bp_throughput(scanner, num, es, bp_input, bp_encoded, bp_output);
            }
            std::cout << es << "," << numThread << "," << fast / MAX_REPEAT << std::endl;
            delete scanner;
        }
    }

    delete[] bp_input;
    free(bp_encoded);
    free(bp_output);
}

