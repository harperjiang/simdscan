//
// Created by Harper on 10/10/17.
//

#ifndef SIMDSCAN_SIMDDELTASCANNER256_H
#define SIMDSCAN_SIMDDELTASCANNER256_H


#include "../Scanner.h"

class SimdDeltaScanner256 : public Scanner {
protected:
    bool shortMode;
public:
    SimdDeltaScanner256(bool);

    virtual ~SimdDeltaScanner256();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_SIMDDELTASCANNER256_H
