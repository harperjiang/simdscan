#include <immintrin.h>
#include <stdio.h>
#include <sys/time.h>

void storeSpeed() {
	struct timeval tp;

	gettimeofday(&tp, NULL);
	printf("Starting %ld\n", tp.tv_sec * 1000 + tp.tv_usec / 1000);
	int* dest = new int[10000000];
	for (int i = 0; i < 2500000; i++) {
		__m128i data = _mm_set1_epi32(5);
		_mm_stream_si128((__m128i *) (dest + i * 4), data);
	}
//	for (int i = 0; i < 10000000; i++) {
//		dest[i] = 5;
//	}

	gettimeofday(&tp, NULL);
	printf("Stopping %ld\n", tp.tv_sec * 1000 + tp.tv_usec / 1000);
}

int main(int argc, char** argv) {
}

