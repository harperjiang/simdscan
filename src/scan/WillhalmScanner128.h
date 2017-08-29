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
protected:
public:
	WillhalmScanner128(int);
	virtual ~WillhalmScanner128();

	void scan(int*, int, int*, Predicate*);
};

#endif /* SRC_SCAN_WILLHALMSCANNER_H_ */
