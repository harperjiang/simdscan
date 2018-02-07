//
// Created by harper on 2/6/18.
//

#ifndef SIMDSCAN_SIMDRLESCANNER_H
#define SIMDSCAN_SIMDRLESCANNER_H


#include "../Scanner.h"

class SIMDRLEScanner : public Scanner {

    SIMDRLEScanner();
    virtual ~SIMDRLEScanner();

    void scan(int*, uint64_t, int*, Predicate*);
};


#endif //SIMDSCAN_SIMDRLESCANNER_H
