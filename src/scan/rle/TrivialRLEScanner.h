//
// Created by harper on 10/19/17.
//

#ifndef SIMDSCAN_TRIVIALRLESCANNER_H
#define SIMDSCAN_TRIVIALRLESCANNER_H


#include "../Scanner.h"

class TrivialRLEScanner : public Scanner {

private:
    int entrySize;
    int rlSize;
public:
    TrivialRLEScanner(int, int);
    virtual ~TrivialRLEScanner();

    // The size here represents numbers of run-length entry
    void scan(int*, uint64_t, int*, Predicate*);
protected:
    void extract(int*, uint64_t, int*, uint32_t*);
};


#endif //SIMDSCAN_TRIVIALRLESCANNER_H
