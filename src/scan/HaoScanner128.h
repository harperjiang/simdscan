/*
 * HaoScanner.h
 *
 * Use AVX2 to perform scanning
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#ifndef SRC_SCAN_HAOSCANNER128_H_
#define SRC_SCAN_HAOSCANNER128_H_

#include <stdlib.h>
#include "Scanner.h"

class HaoScanner128: public Scanner {
private:
	int entrySize;
	bool uload = true;

	int* data = NULL;
	int* dest = NULL;
	int length = 0;
	int val1 = 0;
	int val2 = 0;

public:
	HaoScanner128(int);
	virtual ~HaoScanner128();

	void scan(int*, int, int*, Predicate*);

protected:
	void eq();
	void in();
};

#endif /* SRC_SCAN_HAOSCANNER128_H_ */
