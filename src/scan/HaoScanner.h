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
#include <immintrin.h>
#include "Scanner.h"

class HaoScanner: public Scanner {
private:
	int entrySize;
	bool uload = true;

	int* data = NULL;
	int* dest = NULL;
	int length = 0;

	__m256i* val1s = NULL;
	__m256i* val2s = NULL;
	__m256i* nval1s = NULL;
	__m256i* nval2s = NULL;
	__m256i* nmval1s = NULL;
	__m256i* nmval2s = NULL;
	__m256i* msbmasks = NULL;
	__m256i* notmasks = NULL;
public:
	HaoScanner(int);
	virtual ~HaoScanner();

	void scan(int*, int, int*, Predicate*);

protected:
	void eq();
	void in();
};

#endif /* SRC_SCAN_HAOSCANNER_H_ */
