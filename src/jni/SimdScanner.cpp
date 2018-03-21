#include <stdint.h>
#include <stdlib.h>
#include "edu_uchicago_cs_encsel_query_simdscan_SimdScanner.h"

void scanByByte(jbyte* input, jint offset, jint idx, jint target, jint entryWidth, jbyte* result, jlong& buffer);
void scanInt(jbyte* input, jlong len, jint offset, jint size, jint target, jint entryWidth, jbyte* result);
void scanLong(jbyte* input, jlong len, jint offset, jint size, jint target, jint entryWidth, jbyte* result);

jint getInt(jbyte* buffer, jint index) ;
jlong getLong(jbyte* buffer, jint index) ;
void putInt(jbyte* buffer, jint index, jint value);
void putLong(jbyte* buffer, jint index, jlong value) ;


static void init() __attribute__((constructor));
static void release() __attribute__((destructor));

static jbyte* memblock;
static uint64_t memsize;

void init() {
    memsize = 100*1024*1024;
    memblock = (jbyte*)malloc(memsize*sizeof(jbyte));
}

void release() {
    free(memblock);
}

JNIEXPORT jobject JNICALL Java_edu_uchicago_cs_encsel_query_simdscan_SimdScanner_scanBitpacked
  (JNIEnv *env, jobject self, jobject input, jint offset, jint size, jint target, jint entryWidth) {
   jlong resSize = (size * entryWidth / 64 + (((size * entryWidth) % 64)?1:0)) * 8;

   if(resSize > memsize) {
        memsize = resSize;
        free(memblock);
        memblock = (jbyte*) malloc(sizeof(jbyte)*memsize);
   }

   jbyte* array = (jbyte*)env->GetDirectBufferAddress(input);
   jlong arrayLen = env->GetDirectBufferCapacity(input);

   /**
    * Determine a safe number
    */

   return env->NewDirectByteBuffer(memblock,resSize);

}