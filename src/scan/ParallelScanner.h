//
// Created by harper on 3/21/18.
//

#ifndef SIMDSCAN_PARALLELSCANNER_H
#define SIMDSCAN_PARALLELSCANNER_H

#include <pthread.h>
#include "Scanner.h"

class ParallelScanner : public Scanner {
private:
    int numThread = 0;
    int entrySize = 0;
    Scanner *innerScanner = NULL;

    pthread_t *threadHandles = NULL;
public:
    ParallelScanner(int nt, int es, Scanner *inner);

    virtual ~ParallelScanner();

    void scan(int *, uint64_t, int *, Predicate *);
};


typedef struct _ScannerParam {
    int *input;
    int *output;
    uint64_t size;
    Predicate *p;
    Scanner *scanner;
} ScannerParam;

#endif //SIMDSCAN_PARALLELSCANNER_H
