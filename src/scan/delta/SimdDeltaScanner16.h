//
// Created by Harper on 10/10/17.
//

#ifndef SIMDSCAN_SIMDDELTASCANNER16_H
#define SIMDSCAN_SIMDDELTASCANNER16_H


#include "../Scanner.h"
#include "../../util/unpack/u256/Unpacker.h"
#include <immintrin.h>

class SimdDeltaScanner16 : public Scanner {
protected:
    int entrySize;

    __m512i *shuffleInst;
    __m512i *shiftInst;

    Unpacker *unpacker;

public:
    SimdDeltaScanner16(int es);

    virtual ~SimdDeltaScanner16();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_SIMDDELTASCANNER16_H
