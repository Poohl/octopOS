#include "drivers/dbgu.h"
#include "drivers/vectors.h"
#include "fluff/fluff.h"
#include "apps/demo.h"
#include "libs/printf.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")

extern void init_stacks();
extern void RANDO_CODE ();

int powHex(int e) {
	int ret = 1;
	for (int i = 0; i < e; i++) ret *= 16;
	return ret;
}

int toInt(char* hexStr) {
	int ret = 0;
	int asciiOffset;

	for (int i = 0; i < 8; i++) {
		if (hexStr[i] < 58) 
			asciiOffset = 48;
		else 
			asciiOffset = 87;
		ret += (hexStr[i] - asciiOffset) * powHex(7 - i);
	}
	return ret;
}

void inject_code() {
	int* payload = (int *) 0x26ff00;
	payload[0] = toInt("775f0074");			// illegal instruction

	RANDO_CODE();
}


/* MAIN */
void c_entry(void) {
	/* init driver */
	dbgu_init();
	init_stacks();
	init_vector_handling();
	

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
