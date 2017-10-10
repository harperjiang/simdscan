//
// Created by harper on 10/9/17.
//

#ifndef SIMDSCAN_DELTASCANNER128_H
#define SIMDSCAN_DELTASCANNER128_H


#include "../Scanner.h"

class SimdDeltaScanner128 : public Scanner {
protected:
    bool shortMode;
public:
    SimdDeltaScanner128(bool);
    virtual ~SimdDeltaScanner128();

    void scan(int*, uint64_t, int*, Predicate*);
};


#endif //SIMDSCAN_DELTASCANNER128_H
