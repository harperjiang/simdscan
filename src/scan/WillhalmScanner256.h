/*
 * WillhalmScanner256.h
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#ifndef SRC_SCAN_WILLHALMSCANNER256_H_
#define SRC_SCAN_WILLHALMSCANNER256_H_

#include "Scanner.h"

class WillhalmScanner256: public Scanner {
private:
	int bitSize;
protected:
	int computeShift(int);
public:
	WillhalmScanner256(int);
	virtual ~WillhalmScanner256();

	void scan(int*, int, int*, Predicate*);
};

#endif /* SRC_SCAN_WILLHALMSCANNER256_H_ */
