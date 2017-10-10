//
// Created by harper on 10/9/17.
//

#ifndef SIMDSCAN_TRIVIALDELTASCANNER_H
#define SIMDSCAN_TRIVIALDELTASCANNER_H


#include "../Scanner.h"

class TrivialDeltaScanner : public Scanner {
public:
    TrivialDeltaScanner();

    virtual ~TrivialDeltaScanner();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_TRIVIALDELTASCANNER_H
