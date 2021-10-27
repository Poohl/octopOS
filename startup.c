
#include "memory-map.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")


void c_entry(void) {
	// here be dragons
	int a = 0;
	for (int i = 0; i < 10; ++i) {
		a += i;
	}
	for (;;);
	// we cannot return, because no stack...
}


#pragma GCC pop_options