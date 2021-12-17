#include "misc.h"

void *memset(void *s, int c, size_t n) {
	for (size_t i = 0; i < n; ++i)
		((byte*) s)[i] = c;
	return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
	for (size_t i = 0; i < n; ++i)
		((byte*) dest)[i] = ((byte*) src)[i];
	return dest;
}
