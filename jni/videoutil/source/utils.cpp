#include <string.h>
#include <malloc.h>
#include "utils.h"

unsigned char *chrs_join(const unsigned char *src1, const unsigned char *src2) {
	int len1 = sizeof(src1);
	int len2 = sizeof(src2);
	unsigned char *result = (unsigned char *)malloc(len1 + len2);

	int i;
	for (i=0; i<len1; i++) {
		result[i] = src1[i];
	}
	
	for(i=0; i<len2; i++) {
		result[len1+i] = src2[i];
	}
	return result;
}

unsigned char *chrs_copy(unsigned char *dest, const unsigned char *src, int count) {
	
}
