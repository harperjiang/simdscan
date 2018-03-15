//
// Created by harper on 3/14/18.
//

#ifndef SIMDSCAN_TRIVIALBPSCANNER_H
#define SIMDSCAN_TRIVIALBPSCANNER_H

#include "../Scanner.h"

class TrivialBPScanner : public Scanner {
private:
    int entrySize = 0;
public:

    TrivialBPScanner(int);

    virtual ~TrivialBPScanner();

    void scan(int *, uint64_t, int *, Predicate *);

};


#endif //SIMDSCAN_TRIVIALBPSCANNER_H
