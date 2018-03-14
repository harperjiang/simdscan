//
// Created by Harper on 10/10/17.
//

#ifndef SIMDSCAN_SIMDDELTASCANNER32_H
#define SIMDSCAN_SIMDDELTASCANNER32_H


#include "../Scanner.h"

class SimdDeltaScanner32 : public Scanner {
protected:
    int entrySize;
    int bufferSize;

    __m512i* buffer;
    /**
     * Unpack entries in the given SIMD to buffer
     * @param input SIMD word containing packed entries
     */
    void unpack(__m256i& input, int offset);

public:
    SimdDeltaScanner32(int es);

    virtual ~SimdDeltaScanner32();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_SIMDDELTASCANNER32_H
