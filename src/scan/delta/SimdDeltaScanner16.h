//
// Created by Harper on 10/10/17.
//

#ifndef SIMDSCAN_SIMDDELTASCANNER16_H
#define SIMDSCAN_SIMDDELTASCANNER16_H


#include "../Scanner.h"

class SimdDeltaScanner16 : public Scanner {
protected:
    int entrySize;

    __m512i* shuffleInst;
    __m512i* shiftInst;
    /**
     * Unpack entries in the given SIMD to buffer
     * @param input SIMD word containing packed entries
     */
    __m256i unpack(__m256i& input, int offset);

public:
    SimdDeltaScanner16(int es);

    virtual ~SimdDeltaScanner16();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_SIMDDELTASCANNER16_H
