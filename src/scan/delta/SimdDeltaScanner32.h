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
    SimdDeltaScanner32(uint32_t es);

    virtual ~SimdDeltaScanner32();

    void scan(int *, uint64_t, int *, Predicate *);

    uint32_t getEntrySize();

    __m256i cumsum(__m256i input);
};


#endif //SIMDSCAN_SIMDDELTASCANNER32_H
