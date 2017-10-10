//
// Created by harper on 9/19/17.
//

#ifndef SIMDSCAN_HAOSCANNER512_H
#define SIMDSCAN_HAOSCANNER512_H

#include <immintrin.h>
#include "../Scanner.h"

class HaoScanner512 : public Scanner {
private:
    int entrySize;
    bool aligned = true;

    int *data = NULL;
    int *dest = NULL;
    uint64_t length = 0;

    Predicate *predicate;

    __m512i *val1s = NULL;
    __m512i *val2s = NULL;
    __m512i *nval1s = NULL;
    __m512i *nval2s = NULL;
    __m512i *nmval1s = NULL;
    __m512i *nmval2s = NULL;
    __m512i *msbmasks = NULL;
    __m512i *notmasks = NULL;
public:

    HaoScanner512(int, bool);

    virtual ~HaoScanner512();

    void scan(int *, uint64_t, int *, Predicate *);

protected:

    void alignedEq();

    void alignedIn();

    void unalignedEq();

    void unalignedIn();

};


#endif //SIMDSCAN_HAOSCANNER512_H
