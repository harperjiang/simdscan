//
// Created by harper on 10/19/17.
//

#include "TrivialRLEScanner.h"

TrivialRLEScanner::TrivialRLEScanner(int es, int rls) {
    this->entrySize = es;
    this->rlSize = rls;
}

TrivialRLEScanner::~TrivialRLEScanner() {

}

void TrivialRLEScanner::scan(int *input, uint64_t size, int *output, Predicate *p) {
    // First extract
    switch (p->getOpr()) {
        case opr_eq:
        case opr_neq: {
            int entry;
            uint32_t runLength;
            int *offset = output;
            for (uint64_t index = 0; index < size; index++) {
                extract(input, index, &entry, &runLength);
                *(offset++) = (entry == p->getVal1());
                *(offset++) = runLength;
            }
            break;
        }
        case opr_less:{
            int entry;
            uint32_t runLength;
            int *offset = output;
            for (uint64_t index = 0; index < size; index++) {
                extract(input, index, &entry, &runLength);
                *(offset++) = (entry < p->getVal1());
                *(offset++) = runLength;
            }
            break;
        }
        case opr_in: {
            int entry;
            uint32_t runLength;
            int *offset = output;
            for (uint64_t index = 0; index < size; index++) {
                extract(input, index, &entry, &runLength);
                *(offset++) = (entry >= p->getVal1() && entry < p->getVal2());
                *(offset++) = runLength;
            }
            break;
        }
        default:
            break;
    }
}

int extractEntry(int *input, int index, int offset, int entrySize) {
    int mask = (1 << entrySize) - 1;
    int word0 = *(input + index);
    if (offset + entrySize <= 32) {
        return (word0 >> offset) & mask;
    } else {
        int mask0 = (1 << (32 - offset)) - 1;
        int word1 = *(input + index + 1);
        return ((word0 >> offset & mask0) | word1 << (32 - offset)) & mask;
    }
}

void TrivialRLEScanner::extract(int *input, uint64_t index, int *entry, uint32_t *rl) {
    uint64_t offset = index * (entrySize + rlSize);
    uint64_t entryIndex = offset / 32;
    uint64_t entryOffset = offset % 32;
    uint64_t rlIndex = (offset + entrySize) / 32;
    uint64_t rlOffset = (offset + entrySize) % 32;
    *entry = extractEntry(input, entryIndex, entryOffset, entrySize);
    *rl = extractEntry(input, rlIndex, rlOffset, rlSize);
}