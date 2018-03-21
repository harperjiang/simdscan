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
public:
    HaoLaneLoaderScanner(uint32_t es);

    virtual ~HaoLaneLoaderScanner();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_HAOLANELOADERSCANNER_H
