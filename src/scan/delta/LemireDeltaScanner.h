//
// Created by harper on 3/14/18.
//

#ifndef SIMDSCAN_LEMIREDELTASCANNER_H
#define SIMDSCAN_LEMIREDELTASCANNER_H


#include <cstdint>
#include "../Scanner.h"
#include "../../util/unpack/u256/Unpacker.h"

class LemireDeltaScanner : public Scanner {
private:
    uint32_t entrySize;
    Unpacker* unpacker;
public:
    LemireDeltaScanner(uint32_t es);

    virtual ~LemireDeltaScanner();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_LEMIREDELTASCANNER_H
