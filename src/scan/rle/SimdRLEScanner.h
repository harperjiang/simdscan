//
// Created by harper on 2/6/18.
//

#ifndef SIMDSCAN_SIMDRLESCANNER_H
#define SIMDSCAN_SIMDRLESCANNER_H


#include "../Scanner.h"
#include <immintrin.h>

class SimdRLEScanner : public Scanner {
private:
    int entrySize = 0;
    int rlSize = 0;
    bool aligned = false;
    int groupSize = 0;

    Predicate *predicate = NULL;
    __m512i *input = NULL;
    uint64_t numEntry = 0;
    __m512i *output = NULL;

    __m512i *val1s = NULL;
    __m512i *val2s = NULL;
    __m512i *nval1s = NULL;
    __m512i *nval2s = NULL;
    __m512i *nmval1s = NULL;
    __m512i *nmval2s = NULL;
    __m512i *msbmasks = NULL;

    // entry part is all 1, rl part is all 0
    __m512i *rlmasks = NULL;

    // This one set the rl region all 0
    __m512i *notmasks = NULL;
    // This one set the rl region all 1
public:
    SimdRLEScanner(int es, int rls, bool aligned);

    virtual ~SimdRLEScanner();

    void scan(int *, uint64_t, int *, Predicate *);

protected:
    void equal();
    void aequal();

    void less();
    void aless();
};


#endif //SIMDSCAN_SIMDRLESCANNER_H
