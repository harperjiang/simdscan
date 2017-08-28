/*
 * Predicate.h
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#ifndef PREDICATE_H_
#define PREDICATE_H_

enum Opr {
	opr_eq = 0, opr_neq, opr_in
};

class Predicate {
private:
	Opr opr;
	int val1;
	int val2;
public:
	Predicate(Opr, int, int);
	virtual ~Predicate();

	Opr getOpr();
	int getVal1();
	int getVal2();
};

#endif /* PREDICATE_H_ */
