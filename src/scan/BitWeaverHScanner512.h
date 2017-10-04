//
// Created by Harper on 10/4/17.
//

#ifndef SIMDSCAN_BITWEAVERHSCANNER512_H
#define SIMDSCAN_BITWEAVERHSCANNER512_H


#include "Scanner.h"

class BitWeaverHScanner512 : public Scanner{
private:
    int entrySize;

public:
    BitWeaverHScanner512(int);

    virtual ~BitWeaverHScanner512();

    void scan(int *, uint64_t, int *, Predicate *);
};
};


#endif //SIMDSCAN_BITWEAVERHSCANNER512_H
