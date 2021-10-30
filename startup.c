
#include "memory-map.h"
#include "drivers/dbgu.h"
#include "libs/printf.h"
#include "fluff/fluff.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")

void printfdemo() {
	printf("printf demo mode -- PRESS ANY KEY\n");
	int ret = dbgu_put_byte('#');
	int c = dbgu_get_byte();	
	if (c <= 0) {
		printf("Error %x reading that!\n", c);
		printf("Here are some other nicely formatted things: %p, %x, %c, %s\n", &printfdemo, 0x12, '3', "45");
	} else {
		printf("\nWell done!\nYou entered '%c', acsii index %x \nand it's currently stored at %p in my memory.\n", c, c, &c);
	}	
	printf("And heres what happnes with unknown formats:\n %a %b %% %f\n", 1, 2, 3, 4);	
	printf("done. (press ^a then x to quit)");
}

/* MAIN */
void c_entry(void) {
	/* init driver */
	dbgu_init();

	/* run stuff */
	print_banner();
	printfdemo();

}

#pragma GCC pop_options