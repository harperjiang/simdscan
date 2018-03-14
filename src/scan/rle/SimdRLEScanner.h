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

    uint8_t masks[8] = {0, 1, 2, 4, 8, 16, 32, 64};
    uint8_t invmasks[8] = {0, 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf};
public:
    SimdRLEScanner(int es, int rls, bool aligned);

    virtual ~SimdRLEScanner();

    void scan(int *, uint64_t, int *, Predicate *);

protected:
    void equalNormal();
    void equalFast();
    void aequal();

    void lessFast();
    void lessNormal();
    void aless();
};


#endif //SIMDSCAN_SIMDRLESCANNER_H
