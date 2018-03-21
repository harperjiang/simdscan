//
// Created by harper on 3/20/18.
//

#ifndef SIMDSCAN_HAOLANELOADERSCANNER_H
#define SIMDSCAN_HAOLANELOADERSCANNER_H


#include "../Scanner.h"
#include "../../util/ll/LaneLoader.h"

class HaoLaneLoaderScanner : public Scanner {
private:
    uint32_t entrySize;
    LaneLoader *laneLoader;

    __m512i* val1 = NULL;
    __m512i* notmask = NULL;
    __m512i* mask = NULL;
    __m512i* aornm = NULL;
    __m512i* na = NULL;
public:
    HaoLaneLoaderScanner(uint32_t es);

    virtual ~HaoLaneLoaderScanner();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_HAOLANELOADERSCANNER_H
