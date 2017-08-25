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
public:
	WillhalmScanner();
	virtual ~WillhalmScanner();

	int* scan(int*, Predicate* p);
};

#endif /* SRC_SCAN_WILLHALMSCANNER_H_ */
