//
// Created by harper on 10/9/17.
//

#include "TrivialDeltaScanner.h"

TrivialDeltaScanner::TrivialDeltaScanner(bool shortMode) {
    this->shortMode = shortMode;
}

TrivialDeltaScanner::~TrivialDeltaScanner() {

}

void TrivialDeltaScanner::scan(int *in, uint64_t len, int *out, Predicate *p) {
    if (shortMode)
        return shortScan(in, len, out, p);
    else
        return normalScan(in, len, out, p);
}


void TrivialDeltaScanner::shortScan(int *in, uint64_t len, int *out, Predicate *p) {
    int16_t *shortin = (int16_t *) in;
    int16_t *shortout = (int16_t *) out;

    long current = 0;

    for (uint64_t i = 0; i < len; i++) {
        current += shortin[i];
        switch (p->getOpr()) {
            case opr_eq:
                shortout[i] = current == p->getVal1() ? 0xffff : 0;
                break;
            case opr_neq:
                shortout[i] = current != p->getVal1() ? 0xffff : 0;
                break;
            case opr_less:
                shortout[i] = current < p->getVal1() ? 0xffff : 0;
                break;
            case opr_in:
                shortout[i] = (current <= p->getVal2() && current >= p->getVal1()) ? 0xffff : 0;
                break;
        }
    }
}

void TrivialDeltaScanner::normalScan(int *in, uint64_t len, int *out, Predicate *p) {
    long current = 0;

    for (uint64_t i = 0; i < len; i++) {
        current += in[i];
        switch (p->getOpr()) {
            case opr_eq:
                out[i] = current == p->getVal1() ? 0xffffffff : 0;
                break;
            case opr_neq:
                out[i] = current != p->getVal1() ? 0xffffffff : 0;
                break;
            case opr_less:
                out[i] = current < p->getVal1() ? 0xffffffff : 0;
                break;
            case opr_in:
                out[i] = (current <= p->getVal2() && current >= p->getVal1()) ? 0xffffffff : 0;
                break;
        }
    }
}