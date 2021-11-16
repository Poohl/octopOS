#include "drivers/dbgu.h"
#include "drivers/vectors.h"
#include "fluff/fluff.h"
#include "apps/demo.h"
#include "libs/printf.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")

extern void init_stacks();

void __attribute__((interrupt ("UNDEF"))) illegal_instr_hand() {
	printf("Illegal instruction\r\n");
}

/* MAIN */
void c_entry(void) {
	/* init driver */
	dbgu_init();
	init_stacks();
	set_undef_instr_vector(&illegal_instr_hand);
	switch_vector_table();
	asm(".word 0x775f0074" : : : "memory");
	/* run stuff */
	print_banner();
	io_demo();

}

#pragma GCC pop_options
