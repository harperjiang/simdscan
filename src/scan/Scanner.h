/*
 * Scanner.h
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#ifndef SCANNER_H_
#define SCANNER_H_

#include <cstdint>
#include "../predicate/Predicate.h"


class Scanner {
public:
	Scanner();
	virtual ~Scanner();

	virtual void scan(int*, uint64_t, int*, Predicate*) = 0;
};

#endif /* SCANNER_H_ */
