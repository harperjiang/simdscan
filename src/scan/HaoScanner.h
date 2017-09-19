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
	bool aligned = true;

	int* data = NULL;
	int* dest = NULL;
	uint64_t length = 0;

	Predicate* predicate;

	__m256i* val1s = NULL;
	__m256i* val2s = NULL;
	__m256i* nval1s = NULL;
	__m256i* nval2s = NULL;
	__m256i* nmval1s = NULL;
	__m256i* nmval2s = NULL;
	__m256i* msbmasks = NULL;
	__m256i* notmasks = NULL;
public:
	HaoScanner(int, bool);
	virtual ~HaoScanner();

	void scan(int*, uint64_t, int*, Predicate*);

protected:
	void alignedEq();
	void alignedIn();

	void unalignedEq();
	void unalignedIn();
};

#endif /* SRC_SCAN_HAOSCANNER_H_ */
