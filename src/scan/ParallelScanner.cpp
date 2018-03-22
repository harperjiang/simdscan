//
// Created by harper on 3/21/18.
//

#include "ParallelScanner.h"


static void *scanner_thread(void *param);

ParallelScanner::ParallelScanner(int nt, int es, Scanner *inner) {
    this->numThread = nt;
    this->entrySize = es;
    this->innerScanner = inner;

    this->threadHandles = new pthread_t[numThread];
}

ParallelScanner::~ParallelScanner() {
    delete[] threadHandles;
}

void ParallelScanner::scan(int *input, uint64_t num, int *output, Predicate *p) {
    // Split input and output evenly
    uint64_t numPerThread = num / numThread;
    uint64_t offset = numPerThread * entrySize / 32;

    ScannerParam params[numThread];

    for (int i = 0; i < numThread; i++) {
        params[i].p = p;
        params[i].scanner = innerScanner;
        params[i].input = input + i * offset;
        params[i].output = output + i * offset;
        params[i].size = numPerThread;
        pthread_create(threadHandles + i, NULL, scanner_thread, (void *) (params + i));
    }
    for (int i = 0; i < numThread; i++) {
        pthread_join(threadHandles[i], NULL);
    }
}


void *scanner_thread(void *param) {
    ScannerParam *p = (ScannerParam *) param;
    p->scanner->scan(p->input, p->size, p->output, p->p);
}