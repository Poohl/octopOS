
#include "memory-map.h"
#include "drivers/dbgu.h"
#include "printf.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")


void c_entry(void) {

	
	dbgu_init();
	dbgu_put_byte('#');

	printf("This is a simple test\n");

	printf("This is a more complicated test %c %x %s\n", '1', 0x23, "456");
	printf("This is a more complicated test %c %p %s\n", '1', 0x23, "456");
	printf("This is a more complicated test %n %n %n %c %x %s\n", '1', 0x23, "456");

}

#pragma GCC pop_options