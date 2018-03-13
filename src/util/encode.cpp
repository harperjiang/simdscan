/*
 * Encode.cpp
 *
 *  Created on: Sep 4, 2017
 *      Author: harper
 */

#include <cstdint>
#include "encode.h"

void encode(int *input, int *output, int length, int entrySize) {

    int bitCounter = 0;
    int buffer[] = {0, 0};
    int writeCounter = 0;
    int INT_SIZE = sizeof(int) * 8;

    for (int i = 0; i < length; i++) {
        int nextValue = input[i];

        // Write to lower buffer
        buffer[0] |= (nextValue << bitCounter);
        if (bitCounter + entrySize > INT_SIZE) {
            buffer[1] |= (nextValue >> (INT_SIZE - bitCounter));
        }
        bitCounter += entrySize;
        if (bitCounter >= INT_SIZE) {
            output[writeCounter++] = buffer[0];
            buffer[0] = buffer[1];
            buffer[1] = 0;
            bitCounter -= INT_SIZE;
        }
    }
    output[writeCounter++] = buffer[0];
}

void bitweaverh_encode(int *input, int *output, int length, int entrySize) {
    uint64_t *outputLong = (uint64_t *) output;
    // Each 64 bit will accommodate 64 / (entrySize+1) entry

    uint64_t buffer = 0;
    int offset = 0;
    int outputCounter = 0;
    for (int i = 0; i < length; i++) {
        if (64 - offset < entrySize + 1) {
            outputLong[outputCounter++] = buffer;
            buffer = 0;
            offset = 0;
        }
        buffer |= ((uint64_t) input[i]) << offset;
        offset += entrySize + 1;
    }

    outputLong[outputCounter] = buffer;
}

void encode_rle(int *input, int *output, int length, int entrySize, int rlSize) {
    int writeBuffer = 0;
    int writeCounter = 0;
    int offset = 0;
    int current = input[0];
    int counter = 1;
    for (int i = 1; i < length; i++) {
        if (input[i] != current) {
            // Compose an entry
            writeBuffer |= current << offset;
            offset += entrySize;
            if (offset >= 32) {
                offset -= 32;
                output[writeCounter++] = writeBuffer;
                writeBuffer = current >> (entrySize - offset);
            }

            writeBuffer |= counter << offset;
            offset += rlSize;
            if (offset >= 32) {
                offset -= 32;
                output[writeCounter++] = writeBuffer;
                writeBuffer = counter >> (rlSize - offset);
            }

            current = input[i];
            counter = 1;
        } else {
            counter++;
        }
    }
    writeBuffer |= current << offset;
    offset += entrySize;
    if (offset >= 32) {
        offset -= 32;
        output[writeCounter++] = writeBuffer;
        writeBuffer = current >> (entrySize - offset);
    }

    writeBuffer |= counter << offset;
    offset += rlSize;
    output[writeCounter++] = writeBuffer;
    if (offset >= 32) {
        offset -= 32;
        output[writeCounter] = counter >> (rlSize - offset);
    }
}