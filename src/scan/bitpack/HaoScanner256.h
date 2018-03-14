/*
 * HaoScanner256.h
 *
 * Use AVX2 to perform scanning
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#ifndef SRC_SCAN_HAOSCANNER256_H_
#define SRC_SCAN_HAOSCANNER256_H_

#include <stdlib.h>
#include <immintrin.h>
#include "../Scanner.h"

class HaoScanner256: public Scanner {
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


	uint8_t masks[8] = {0, 1, 2, 4, 8, 16, 32, 64};
	uint8_t invmasks[8] = {0, 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf};
public:
	HaoScanner256(int, bool);
	virtual ~HaoScanner256();

	void scan(int*, uint64_t, int*, Predicate*);

protected:
	void alignedEq();
	void alignedLess();

	void unalignedEq();
	void unalignedLess();
};

#endif /* SRC_SCAN_HAOSCANNER256_H_ */
