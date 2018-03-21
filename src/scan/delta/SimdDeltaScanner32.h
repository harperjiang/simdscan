//
// Created by Harper on 10/10/17.
//

#ifndef SIMDSCAN_SIMDDELTASCANNER32_H
#define SIMDSCAN_SIMDDELTASCANNER32_H


#include "../Scanner.h"
#include "../../util/unpack/u256/Unpacker.h"
#include <immintrin.h>

class SimdDeltaScanner32 : public Scanner {
protected:
    int entrySize;

    __m512i* shuffleInst;
    __m512i* shiftInst;

    Unpacker* unpacker;
public:
    SimdDeltaScanner32(int es);

    virtual ~SimdDeltaScanner32();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_SIMDDELTASCANNER32_H
