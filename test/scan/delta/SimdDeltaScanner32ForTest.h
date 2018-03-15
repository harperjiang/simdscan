//
// Created by harper on 3/15/18.
//

#ifndef SIMDSCAN_SIMDDELTASCANNER32FORTEST_H
#define SIMDSCAN_SIMDDELTASCANNER32FORTEST_H


#include <avxintrin.h>
#include "../../../src/scan/delta/SimdDeltaScanner32.h"

class SimdDeltaScanner32ForTest : public SimdDeltaScanner32 {

public:
    SimdDeltaScanner32ForTest(int es) : SimdDeltaScanner32(es) {

    }

    __m256i unpackForTest(__m256i input, int offset) {
        unpack(input, offset);
    }
};


#endif //SIMDSCAN_SIMDDELTASCANNER32FORTEST_H
