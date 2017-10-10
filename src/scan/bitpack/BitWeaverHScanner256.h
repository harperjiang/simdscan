//
// Created by Harper on 10/4/17.
//

#ifndef SIMDSCAN_BITWEAVERHSCANNER256_H
#define SIMDSCAN_BITWEAVERHSCANNER256_H


#include "../Scanner.h"

class BitWeaverHScanner256 : public Scanner {
private:
    int entrySize;

public:
    BitWeaverHScanner256(int);

    virtual ~BitWeaverHScanner256();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_BITWEAVERHSCANNER256_H
