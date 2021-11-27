#include "drivers/dbgu.h"
#include "drivers/vectors.h"
#include "fluff/fluff.h"
#include "apps/demo.h"
#include "libs/printf.h"
#include "apps/injection_trainer.h"
#include "drivers/aic.h"
#include "memory-map.h"


// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")

extern void init_stacks();

/* MAIN */
void c_entry(void) {
	/* init driver */
	dbgu_init();
	init_stacks();
	init_vector_handling();

	asm volatile (
		"MRS r0,CPSR\n"
		"BIC r0,r0,#0b11000000\n"
		"MSR cpsr, r0\n"
		: : : "r0"); // #NOTSORRY for windows plebs

	volatile aic* _aic = (aic*) AIC;

	_aic->enable = 0b100;
	_aic->set = 0b100;

	print_banner();
	injection_trainer();

	/* run stuff 
	print_banner();
	asm(".word 0x775f0074" : : : "memory");
	io_demo();
	*/

}

#pragma GCC pop_options
