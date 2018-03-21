//
// Created by harper on 3/20/18.
//

#ifndef SIMDSCAN_WILLHALMUNPACKERSCANNER_H
#define SIMDSCAN_WILLHALMUNPACKERSCANNER_H


#include "../Scanner.h"
#include "../../util/unpack/u256/Unpacker.h"

class WillhalmUnpackerScanner : public Scanner {
private:
    Unpacker *unpacker;
    uint64_t entrySize;
public:
    WillhalmUnpackerScanner(int es);

    virtual ~WillhalmUnpackerScanner();


    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_WILLHALMUNPACKERSCANNER_H
