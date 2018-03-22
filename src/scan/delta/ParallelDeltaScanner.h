//
// Created by harper on 3/22/18.
//

#ifndef SIMDSCAN_PARALLELDELTASCANNER_H
#define SIMDSCAN_PARALLELDELTASCANNER_H


#include "SimdDeltaScanner32.h"

class ParallelDeltaScanner : public Scanner {
private:
    SimdDeltaScanner32 *innerScanner = NULL;
    uint32_t numThread = 0;
    pthread_t* threadHandles;

public:
    ParallelDeltaScanner(uint32_t numThread, uint32_t es);
    virtual ~ParallelDeltaScanner();

    void scan(int*, uint64_t, int*, Predicate*);
};


#endif //SIMDSCAN_PARALLELDELTASCANNER_H
