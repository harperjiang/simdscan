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
	int entrySize;
	bool aligned = false;

	int* data;
	int length;
	Predicate* p;

	int* dest;
	int destCounter;
	int buffer[4];
	int bufferCounter;
protected:
	void writeToDest(__m128i, int);
	void writeToDest(int);


	void scanAligned();
	void scanUnaligned();
public:
	WillhalmScanner128(int);
	virtual ~WillhalmScanner128();

	void scan(int*, int, int*, Predicate*);
};

#endif /* SRC_SCAN_WILLHALMSCANNER_H_ */
