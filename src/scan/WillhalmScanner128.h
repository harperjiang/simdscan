/*
 * WillhalmScanner.h
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#ifndef SRC_SCAN_WILLHALMSCANNER128_H_
#define SRC_SCAN_WILLHALMSCANNER128_H_

#include <immintrin.h>
#include "Scanner.h"

class WillhalmScanner128 : public Scanner {
private:
    int entrySize;
    bool aligned = true;

    int *data;
    int *dest;
    int length;
    Predicate *p;

    int destCounter;
    int buffer[4];
    int bufferCounter;
protected:
    void writeToDest(__m128i, int);

    void writeToDest(int);


    void scanAligned();

    void scanUnaligned();

    void compareAndWrite(__m128i, __m128i, __m128i, __m128i, __m128i, int);

public:
    WillhalmScanner128(int, bool);

    virtual ~WillhalmScanner128();

    void scan(int *, int, int *, Predicate *);
};

#endif /* SRC_SCAN_WILLHALMSCANNER_H_ */
