/*
 * Predicate.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: harper
 */

#include "Predicate.h"

Predicate::Predicate(Opr opr, int val1, int val2) {
	this->opr = opr;
	this->val1 = val1;
	this->val2 = val2;
}

Predicate::~Predicate() {

}

Opr Predicate::getOpr() {
	return this->opr;
}

int Predicate::getVal1() {
	return this->val1;
}

int Predicate::getVal2() {
	return this->val2;
}
