
#include "memory-map.h"
#include "drivers/dbgu.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")


void c_entry(void) {

	tst();
	// print("wtf?");
}


#pragma GCC pop_options