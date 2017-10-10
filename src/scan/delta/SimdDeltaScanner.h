//
// Created by harper on 10/9/17.
//

#ifndef SIMDSCAN_DELTASCANNER_H
#define SIMDSCAN_DELTASCANNER_H


#include "../Scanner.h"

class SimdDeltaScanner : public Scanner {
public:
    SimdDeltaScanner();
    virtual ~SimdDeltaScanner();

    void scan(int*, uint64_t, int*, Predicate*);
};


#endif //SIMDSCAN_DELTASCANNER_H
