/**
 *  Use AvX-512 to perform scanning
 *
 *  Created by harper on 9/19/17.
 */

#ifndef SIMDSCAN_HAOSCANNER512_H
#define SIMDSCAN_HAOSCANNER512_H

#include <immintrin.h>
#include "../Scanner.h"

class HaoScanner512 : public Scanner {
private:
    int entrySize;

    int *data = NULL;
    int *dest = NULL;
    uint64_t length = 0;

    Predicate *predicate;

    __m512i *val1s = NULL;
    __m512i *nval1s = NULL;
    __m512i *nmval1s = NULL;
    __m512i *msbmasks = NULL;
    __m512i *notmasks = NULL;

    uint8_t masks[8] = {0, 1, 2, 4, 8, 16, 32, 64};
    uint8_t invmasks[8] = {0, 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf};
public:

    HaoScanner512(int);

    virtual ~HaoScanner512();

    void scan(int *, uint64_t, int *, Predicate *);

protected:
    void equal();
    void less();

};


#endif //SIMDSCAN_HAOSCANNER512_H
