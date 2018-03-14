//
// Created by harper on 10/9/17.
//

#ifndef SIMDSCAN_TRIVIALDELTASCANNER_H
#define SIMDSCAN_TRIVIALDELTASCANNER_H


#include "../Scanner.h"

class TrivialDeltaScanner : public Scanner {
protected:
    int entrySize;

    // Scan 16 bit
    void shortScan(int *, uint64_t, int *, Predicate *);

    // Scan 32 bit
    void normalScan(int *, uint64_t, int *, Predicate *);

public:
    TrivialDeltaScanner(int);

    virtual ~TrivialDeltaScanner();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_TRIVIALDELTASCANNER_H
