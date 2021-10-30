#include "demo.h"
#include "memory-map.h"
#include "drivers/dbgu.h"
#include "libs/printf.h"

void io_demo() {
	printf("printf demo mode -- PRESS ANY KEY\n");
	int ret = dbgu_put_byte('#');
	int c = dbgu_get_byte();	
	if (c <= 0) {
		printf("Error %x reading that!\n", c);
		printf("Here are some other nicely formatted things: %p, %x, %c, %s\n", &io_demo, 0x12, '3', "45");
	} else {
		printf("\nWell done!\nYou entered '%c', acsii index %x \nand it's currently stored at %p in my memory.\n", c, c, &c);
	}	
	printf("And heres what happnes with unknown formats:\n %a %b %% %f\n", 1, 2, 3, 4);	
	printf("done. (press ^a then x to quit)");
}