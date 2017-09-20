/*
 * WillhalmScanner256.h
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#ifndef SRC_SCAN_WILLHALMSCANNER256_H_
#define SRC_SCAN_WILLHALMSCANNER256_H_

#include <immintrin.h>
#include "Scanner.h"

class WillhalmScanner256: public Scanner {
private:
	int entrySize;
	bool aligned = true;

	int *data;
	int *dest;
	uint64_t length;
	Predicate *p;

	int destCounter;
	int buffer[8];
	int bufferCounter;
protected:
	void writeToDest(__m256i, int);

	void writeToDest(int);


	void scanAligned();

	void scanUnaligned();

	void compareAndWrite(__m256i, __m256i, __m256i, __m256i, __m256i, int);

public:
	WillhalmScanner256(int, bool);

	virtual ~WillhalmScanner256();

	void scan(int *, uint64_t, int *, Predicate *);
};

#endif /* SRC_SCAN_WILLHALMSCANNER256_H_ */
