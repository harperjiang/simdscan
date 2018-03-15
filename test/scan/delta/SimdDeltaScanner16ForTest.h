//
// Created by harper on 3/15/18.
//

#ifndef SIMDSCAN_SIMDDELTASCANNER16FORTEST_H
#define SIMDSCAN_SIMDDELTASCANNER16FORTEST_H


#include <immintrin.h>
#include "../../../src/scan/delta/SimdDeltaScanner16.h"

class SimdDeltaScanner16ForTest : public SimdDeltaScanner16 {
public:
    SimdDeltaScanner16ForTest(int es) : SimdDeltaScanner16(es) {
    }

    ~SimdDeltaScanner16ForTest() {

    }

    __m256i unpackForTest(__m256i input, int offset) {
        return unpack(input,offset);
    }
};


#endif //SIMDSCAN_SIMDDELTASCANNER16FORTEST_H
