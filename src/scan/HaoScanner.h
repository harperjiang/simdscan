/*
 * HaoScanner.h
 *
 * Use AVX2 to perform scanning
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#ifndef SRC_SCAN_HAOSCANNER_H_
#define SRC_SCAN_HAOSCANNER_H_

#include <stdlib.h>
#include "Scanner.h"

class HaoScanner: public Scanner {
private:
	int bitSize;

	int* data = NULL;
	int* dest = NULL;
	int length = 0;
	int val1 = 0;
	int val2 = 0;

public:
	HaoScanner(int);
	virtual ~HaoScanner();

	void scan(int*, int, int*, Predicate*);

protected:
	void eq();
	void in();
};

#endif /* SRC_SCAN_HAOSCANNER_H_ */
