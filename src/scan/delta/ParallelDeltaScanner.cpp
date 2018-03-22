//
// Created by harper on 3/22/18.
//
#include "../ParallelScanner.h"
#include "ParallelDeltaScanner.h"

typedef struct _AddParam {
    int *data;
    int addend;
    int size;
} AddParam;

static void *parallel_scan(void *);

static void *parallel_add(void *);

ParallelDeltaScanner::ParallelDeltaScanner(uint32_t numThread, uint32_t es) {
    this->innerScanner = new SimdDeltaScanner32(es);
    this->numThread = numThread;
    this->threadHandles = new pthread_t[numThread];
}

ParallelDeltaScanner::~ParallelDeltaScanner() {
    delete this->innerScanner;
    delete[] this->threadHandles;
}

void ParallelDeltaScanner::scan(int *input, uint64_t num, int *output, Predicate *p) {
    uint32_t entrySize = this->innerScanner->getEntrySize();
    uint64_t numPerThread = num / numThread;
    uint64_t offset = numPerThread * entrySize / 32;

    ScannerParam params[numThread];

    for (int i = 0; i < numThread; i++) {
        params[i].p = p;
        params[i].scanner = innerScanner;
        params[i].input = input + i * offset;
        params[i].output = output + i * offset;
        params[i].size = numPerThread;
        pthread_create(threadHandles + i, NULL, parallel_scan, (void *) (params + i));
    }
    for (int i = 0; i < numThread; i++) {
        pthread_join(threadHandles[i], NULL);
    }

    AddParam aParams[numThread];
    // Collect sum
    aParams[0].addend = 0;
    for (int i = 1; i < numThread; i++) {
        aParams[i].addend = aParams[i - 1].addend + output[offset * i - 1];
        aParams[i].data = output + i * offset;
        aParams[i].size = numPerThread;
        pthread_create(threadHandles + i, NULL, parallel_add, (void *) (aParams + i));
    }
    for (int i = 1; i < numThread; i++) {
        pthread_join(threadHandles[i], NULL);
    }
}

void *parallel_scan(void *param) {
    ScannerParam *p = (ScannerParam *) param;
    p->scanner->scan(p->input, p->size, p->output, p->p);
}

void *parallel_add(void *param) {
    AddParam *ap = (AddParam *) param;
    __m512i *simdo = (__m512i *) ap->data;
    __m512i add = _mm512_set1_epi32(ap->addend);
    uint64_t round = ap->size / 16;
    for (uint64_t i = 0; i < round; i++) {
        __m512i load = _mm512_loadu_si512(simdo + i);
        __m512i added = _mm512_add_epi32(load, add);
        _mm512_storeu_si512(simdo + i, added);
    }
}