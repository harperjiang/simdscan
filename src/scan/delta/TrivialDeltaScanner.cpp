//
// Created by harper on 10/9/17.
//

#include "TrivialDeltaScanner.h"
#include "../../util/encode.h"

TrivialDeltaScanner::TrivialDeltaScanner(int es) {
    this->entrySize = es;
}

TrivialDeltaScanner::~TrivialDeltaScanner() {

}

void TrivialDeltaScanner::scan(int *in, uint64_t len, int *out, Predicate *p) {

    uint16_t current = 0;
    uint64_t byteoff = 0;
    uint8_t bitoff = 0;

    uint64_t outintoff = 0;
    uint8_t outbitoff = 0;

    for (uint64_t i = 0; i < len; i++) {
        current += extract_entry(in, byteoff, bitoff, entrySize);
        switch (p->getOpr()) {
            case opr_eq:
                out[outintoff] |= (current == p->getVal1()) << outbitoff++;
                break;
            case opr_neq:
                out[outintoff] |= (current != p->getVal1()) << outbitoff++;
                break;
            case opr_less:
                out[outintoff] |= (current < p->getVal1()) << outbitoff++;
                break;
            default:
                break;
        }
        if (outbitoff == 32) {
            outbitoff = 0;
            outintoff++;
        }
        bitoff += entrySize;
        byteoff += bitoff / 8;
        bitoff %= 8;
    }
}

