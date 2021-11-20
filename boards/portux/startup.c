#include "drivers/dbgu.h"
#include "drivers/vectors.h"
#include "fluff/fluff.h"
#include "apps/demo.h"
#include "libs/printf.h"
#include "apps/injection_trainer.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")

extern void init_stacks();
extern void RANDO_CODE ();

int toInt(char* hexStr) {
	int ret = 0;
	int asciiOffset;

	for (int i = 0; i < 8; i++) {
		if (hexStr[i] < 58) 
			asciiOffset = 48;
		else 
			asciiOffset = 87;
		ret += (hexStr[i] - asciiOffset) * (1 << 4*(7-i));
	}
	return ret;
}

void inject_code() {
	int* payload = (int *) 0x26ff00;
	char* buffer;
	int instrcnt = 0;
	int i;
	//payload[0] = toInt("775f0074");			// illegal instruction

	/* read input */
	while (buffer[i] != '$') {
		printf("\n\rinject @ %p -> 0x", &payload[instrcnt]);
		for (i = 0; i < 8; i++) {
			buffer[i] = dbgu_get_byte();

			/* backspace function */
			if (buffer[i] == 0x7f) {
				printf("\n\rinject @ %p -> 0x", &payload[instrcnt]);
				for (int j = 0; j < i-1; j++) dbgu_put_byte(buffer[j]);
				i -= 2;
				continue;
			};

			dbgu_put_byte(buffer[i]);
			if (buffer[i] == '$') break;
		} 

		if (buffer[i] != '$') payload[instrcnt] = toInt(buffer);
		instrcnt++;
	}
	printf("\n\rhere goes nothing...\n\r");
	RANDO_CODE();
}


/* MAIN */
void c_entry(void) {
	/* init driver */
	dbgu_init();
	init_stacks();
	init_vector_handling();
	
	injection_trainer();

	inject_code();

	/*
	asm(".word 0x775f0074" : : : "memory");
	/* run stuff 
	print_banner();
	asm(".word 0x775f0074" : : : "memory");
	io_demo();
	*/

}

#pragma GCC pop_options
