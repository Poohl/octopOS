#include "demo.h"
#include "libs/hardware.h"
#include "libs/printf.h"

void io_demo() {
	int c;
	printf("printf demo mode -- PRESS ANY KEY\n\r");
	printf("(press $ to quit)\n\r");
	do {
		int ret = debug_put_char('#');
		c = debug_get_char();	
		if (c <= 0) {
			printf("Error %x reading that!\n\r", c);
			printf("Here are some other nicely formatted things: %p, %x, %c, %s\n\r", &io_demo, 0x12, '3', "45");
		} else {
			printf("\n\rWell done!\n\rYou entered '%c', acsii index %x \n\rand it's currently stored at %p in my memory.\n\r", c, c, &c);
		}	

	} while (c != '$');

	printf("end of demo loop.\n\r");
	//printf("And heres what happnes with unknown formats:\n\r %a %b %% %f\n\r", 1, 2, 3, 4);	
	printf("done. (press ^a then x to quit qemu)");
}
