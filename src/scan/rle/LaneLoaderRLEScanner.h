//
// Created by harper on 3/22/18.
//

#ifndef SIMDSCAN_LANELOADERRLESCANNER_H
#define SIMDSCAN_LANELOADERRLESCANNER_H


#include "../Scanner.h"
#include "../../util/ll/LaneLoader.h"

class LaneLoaderRLEScanner : public Scanner {
private:
    uint32_t entrySize;
    uint32_t rlSize;
    LaneLoader *laneLoader;

    __m512i* val1 = NULL;
    __m512i* notmask = NULL;
    __m512i* mask = NULL;
    __m512i* aornm = NULL;
    __m512i* na = NULL;
public:
    LaneLoaderRLEScanner(uint32_t es, uint32_t rls);

    virtual ~LaneLoaderRLEScanner();

    void scan(int *, uint64_t, int *, Predicate *);
};


#endif //SIMDSCAN_LANELOADERRLESCANNER_H
