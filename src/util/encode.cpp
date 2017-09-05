/*
 * Encode.cpp
 *
 *  Created on: Sep 4, 2017
 *      Author: harper
 */

void encode(int* input, int* output, int length, int entrySize) {

	int bitCounter = 0;
	int buffer[] = { 0, 0 };
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

