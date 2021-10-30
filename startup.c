
#include "memory-map.h"
#include "drivers/dbgu.h"
#include "printf.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")


void c_entry(void) {
	dbgu_init();
	int ret = dbgu_put_byte('#');

	printf("Please enter a character to get some info:\n");

	int c = dbgu_get_byte();

	if (c <= 0) {
		printf("Error %x reading that!\n", c);
		printf("Here are some other nicely formatted things: %p, %x, %c, %s\n", &c_entry, 0x12, '3', "45");
	} else {
		printf("You entered a %c, acsii index %x and it's currently stored at %p in my memory\n", c, c, &c);
	}

	printf("And heres what happnes with unknown formats: %a %b %% %f\n", 1, 2, 3, 4);

	printf("done");
}

#pragma GCC pop_options