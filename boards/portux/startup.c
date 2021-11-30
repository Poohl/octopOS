#include "drivers/dbgu.h"
#include "drivers/vectors.h"
#include "drivers/system_timer.h"
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

__attribute__((interrupt ("IRQ")))
void system_interrupt_hand() {
	dbgu_interupt_callback();
	timer_interrupt_callback();
	enable_interrupts();
}

/* MAIN */
void c_entry(void) {
	/* init driver */
	dbgu_init();
	init_stacks();
	init_vector_handling();

	enable_interrupts();

	set_interrupt_handler(1, &system_interrupt_hand, 0, 0);
	set_timer_interval(1000);

	volatile aic* inter_tset = (aic*) AIC;

	inter_tset->enable = 2;
	//inter_tset->set = 0b10000;

	volatile if_hw_mem_dbgu* dbgu_test = (if_hw_mem_dbgu*) DBGU;

	dbgu_test->interrupt_disable = 0xFFFFFFFF;
	dbgu_test->interrupt_enable = 1;

	print_banner();
	injection_trainer();

	/* run stuff 
	print_banner();
	asm(".word 0x775f0074" : : : "memory");
	io_demo();
	*/

}

#pragma GCC pop_options
