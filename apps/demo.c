#include "demo.h"
#include "libs/hardware.h"
#include "libs/printf.h"

void io_demo() {
	int c;
	printf("printf demo mode -- PRESS ANY KEY\n");
	printf("(press $ to quit)\n");
	do {
		int ret = debug_put_char('#');
		c = debug_get_char();	
		if (c <= 0) {
			printf("Error %x reading that!\n", c);
			printf("Here are some other nicely formatted things: %p, %x, %c, %s\n", &io_demo, 0x12, '3', "45");
		} else {
			printf("\nWell done!\nYou entered '%c', acsii index %x \nand it's currently stored at %p in my memory.\n", c, c, &c);
		}	

	} while (c != '$');

	printf("end of demo loop.\n");
	printf("And heres what happnes with unknown formats:\n %a %b %% %f\n", 1, 2, 3, 4);	
	printf("done. (press ^a then x to quit qemu)");
}