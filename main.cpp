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

void uloadSpeed() {

	__m256i result = _mm256_set1_epi32(8);

	struct timeval tp;

	gettimeofday(&tp, NULL);
	printf("Starting %ld\n", tp.tv_sec * 1000 + tp.tv_usec / 1000);
	// Aligned Load
	int* aligned = (int*) aligned_alloc(2048, 8000000 * sizeof(int));
	for (int i = 0; i < 1000000; i++) {
		__m256i a = _mm256_load_si256((__m256i *) (aligned + i * 8));
		result = _mm256_xor_si256(a, result);
	}
	gettimeofday(&tp, NULL);
	printf("Stopping %ld\n", tp.tv_sec * 1000 + tp.tv_usec / 1000);
	// Unaligned load
	int* unaligned = new int[1000007];
	for (int i = 0; i < 1000000; i++) {
		__m256i a = _mm256_loadu_si256((__m256i *) (unaligned + i));
		result = _mm256_xor_si256(a, result);
	}
	gettimeofday(&tp, NULL);
	printf("Stopping %ld\n", tp.tv_sec * 1000 + tp.tv_usec / 1000);

	for (int i = 0; i < 1000000; i++) {
		__m256i a = _mm256_stream_load_si256((__m256i *) (aligned + i * 8));
		result = _mm256_xor_si256(a, result);
	}
	gettimeofday(&tp, NULL);
	printf("Stopping %ld\n", tp.tv_sec * 1000 + tp.tv_usec / 1000);

	printf("%d\n", _mm256_extract_epi32(result, 0));
}

int main(int argc, char** argv) {
	uloadSpeed();
}

