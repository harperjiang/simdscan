//
// Created by harper on 10/19/17.
//

#ifndef SIMDSCAN_TRIVIALRLESCANNER_H
#define SIMDSCAN_TRIVIALRLESCANNER_H


#include "../Scanner.h"

class TrivialRLEScanner : public Scanner {
    TrivialRLEScanner();
    virtual ~TrivialRLEScanner();

    void scan(int*, uint64_t, int*, Predicate*);
};


#endif //SIMDSCAN_TRIVIALRLESCANNER_H
