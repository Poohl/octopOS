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
