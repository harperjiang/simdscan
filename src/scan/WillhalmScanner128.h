/*
 * WillhalmScanner.h
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#ifndef SRC_SCAN_WILLHALMSCANNER_H_
#define SRC_SCAN_WILLHALMSCANNER_H_

#include "Scanner.h"

class WillhalmScanner: public Scanner {
private:
	int bitSize;
protected:
	int computeShift(int);
public:
	WillhalmScanner(int);
	virtual ~WillhalmScanner();

	void scan(int*, int, int*, Predicate*);
};

#endif /* SRC_SCAN_WILLHALMSCANNER_H_ */
