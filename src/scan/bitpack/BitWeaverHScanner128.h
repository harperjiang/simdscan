//
// Created by Harper on 10/3/17.
//

#ifndef SIMDSCAN_BITWEAVERHSCANNER128_H
#define SIMDSCAN_BITWEAVERHSCANNER128_H


#include "../Scanner.h"

class BitWeaverHScanner128 : public Scanner {

private:
    int entrySize;

public:
    BitWeaverHScanner128(int);

    virtual ~BitWeaverHScanner128();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_BITWEAVERHSCANNER128_H
