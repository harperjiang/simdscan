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

class WillhalmScanner128: public Scanner {
private:
	int bitSize;
	int shuffleCount;
	__m128i* shuffles;
	__m128i* compareMask;
	__m128i* val1;
	__m128i* val2;
protected:
	int computeShift(int);
	void computePredicate(Predicate*);
public:
	WillhalmScanner128(int);
	virtual ~WillhalmScanner128();

	void scan(int*, int, int*, Predicate*);
};

#endif /* SRC_SCAN_WILLHALMSCANNER_H_ */
