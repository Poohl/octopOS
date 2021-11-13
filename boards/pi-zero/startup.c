//#include "memory-map.h"
//#include "drivers/dbgu.h"
#include "libs/printf.h"
#include "fluff/fluff.h"
// #include "apps/demo.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")


/* MAIN */
void c_entry(void) {

	/* run stuff */
	print_banner();
	printf("(pi-zero edition)\n\n\r");
	io_demo();

}

#pragma GCC pop_options