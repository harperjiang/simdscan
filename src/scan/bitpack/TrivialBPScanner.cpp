//
// Created by harper on 3/14/18.
//

#include "TrivialBPScanner.h"
#include "../util/encode.h"

TrivialBPScanner::TrivialBPScanner512(int es) {
    this->entrySize = es;
}

virtual TrivialBPScanner::~TrivialBPScanner512() {

}

void TrivialBPScanner::scan(int *input, uint64_t numEntry, int *output, Predicate *p) {

    uint32_t outbuffer = 0;
    uint64_t outindex = 0;
    int outoffset = 0;

    for (uint64_t index = 0; index < numEntry; index++) {
        int intidx = index * entrySize / 32;
        int intoffset = index * entrySize % 32;
        int number = extract_entry(input, intidx, intoffset, entrySize);

        int res = 0;
        switch (p->getOpr()) {
            case opr_eq:
            case opr_neq:
                res = number != p->getVal1();
                break;
            case opr_less:
                res = number >= p->getVal1();
                break;
            default:
                break;
        }

        outbuffer |= (res << outindex++);
        if (outindex == 32) {
            outindex = 0;
            output[outindex++] = outbuffer;
            outbuffer = 0;
        }
    }
}