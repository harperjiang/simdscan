#include <stdint.h>
#include <stdlib.h>
#include "edu_uchicago_cs_encsel_query_simdscan_SimdScanner.h"
#include "../scan/bitpack/HaoLaneLoaderScanner.h"
#include "../decode/BPDecoder.h"

void scanByByte(jbyte *input, jint offset, jint idx, jint target, jint entryWidth, jbyte *result, jlong &buffer);

void scanInt(jbyte *input, jlong len, jint offset, jint size, jint target, jint entryWidth, jbyte *result);

void scanLong(jbyte *input, jlong len, jint offset, jint size, jint target, jint entryWidth, jbyte *result);

jint getInt(jbyte *buffer, jint index);

jlong getLong(jbyte *buffer, jint index);

void putInt(jbyte *buffer, jint index, jint value);

void putLong(jbyte *buffer, jint index, jlong value);


static void init() __attribute__((constructor));

static void release() __attribute__((destructor));

static jbyte *memblock;
static uint64_t memsize;

void init() {
    memsize = 100 * 1024 * 1024;
    memblock = (jbyte *) aligned_alloc(64, memsize * sizeof(jbyte));
}

void release() {
    free(memblock);
}

void allocOutput(jlong outputSize) {
    if (outputSize > memsize) {
        memsize = outputSize;
        free(memblock);
        memblock = (jbyte *) aligned_alloc(64, sizeof(jbyte) * memsize);
    }
}

JNIEXPORT jobject JNICALL Java_edu_uchicago_cs_encsel_query_simdscan_SimdScanner_scanBitpacked
        (JNIEnv *env, jobject self, jobject input, jint offset, jint size, jint target, jint entryWidth) {
    jbyte *array = (jbyte *) env->GetDirectBufferAddress(input);
    jlong arrayLen = env->GetDirectBufferCapacity(input);

    jint safeSize = (arrayLen - 1024) / entryWidth;

    // Allocate a 64-bit for each entry, should be enough
    allocOutput(safeSize * 8);

    Scanner *scanner = new HaoLaneLoaderScanner(entryWidth);
    scanner->scan((int *) array, safeSize, (int *) memblock, new Predicate(opr_eq, target, 0));
    /**
     * Determine a safe number to leave enough space for Scanner to read, e.g. 2 SIMD words
     */


    return env->NewDirectByteBuffer(memblock, safeSize * 8);

}

JNIEXPORT jobject JNICALL Java_edu_uchicago_cs_encsel_query_simdscan_SimdScanner_decodeBitpacked
        (JNIEnv *env, jobject self, jobject input, jint offset, jint size, jint entryWidth) {
    jbyte *array = (jbyte *) env->GetDirectBufferAddress(input);
    jlong arrayLen = env->GetDirectBufferCapacity(input);

    jint safeSize = (arrayLen - 1024) / entryWidth;

    Decoder *decoder = new BPDecoder(entryWidth);

    // Allocate a 64-bit for each entry, should be enough
    jint numSafeBytes = (safeSize / 8 + 1) * 32;
    allocOutput((safeSize / 8 + 1) * 32);

    decoder->decode((int *) array, safeSize, (int *) memblock);

    return env->NewDirectByteBuffer(memblock, numSafeBytes);
}