//
// Created by Harper on 10/10/17.
//

#ifndef SIMDSCAN_SIMDDELTASCANNER32_H
#define SIMDSCAN_SIMDDELTASCANNER32_H


#include "../Scanner.h"
#include <immintrin.h>

class SimdDeltaScanner32 : public Scanner {
protected:
    int entrySize;

    __m512i* shuffleInst;
    __m512i* shiftInst;
    /**
     * Unpack entries in the given SIMD to buffer
     * @param input SIMD word containing packed entries
     */
    __m256i unpack(__m256i &input, int offset);

public:
    SimdDeltaScanner32(int es);

    virtual ~SimdDeltaScanner32();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_SIMDDELTASCANNER32_H
