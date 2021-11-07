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
	/* init driver */
	//dbgu_init();

	/* run stuff */
	print_banner();
	//io_demo();
	//printf("Hello word");

}

#pragma GCC pop_options