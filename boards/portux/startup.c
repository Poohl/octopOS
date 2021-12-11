#include "drivers/dbgu.h"
#include "drivers/vectors.h"
#include "drivers/system_timer.h"
#include "fluff/fluff.h"
#include "apps/demo.h"
#include "libs/printf.h"
#include "apps/injection_trainer.h"
#include "drivers/aic.h"
#include "board.h"
#include "libs/delay.h"
#include "drivers/cpu.h"


// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")

extern void init_stacks();

extern void system_interrupt_hand();

/* MAIN */
void c_entry(void) {
	/* init  */
	dbgu_init();
	init_stacks();
	init_vector_handling();

	/* interrups */
	u32 buff;

	asm("mrs %0, cpsr" :  "=r" (buff) : : );
	printf("No Interrupts:\r\n%x\r\n", buff);
	cpu_init();

	enable_interrupts();

	asm("mrs %0, cpsr" :  "=r" (buff) : : );
	printf("Yes Interrupts\r\n%x\r\n", buff);

	set_interrupt_handler(1, &system_interrupt_hand, 0, 0);

	/* system clock */
	/*    SET INTERVAL HERE
						||
						||
					   \  /
					    \/			*/
	set_timer_interval(10000);

	/* interrupt enable @ aic (2)*/
	volatile aic* inter_tset = (aic*) AIC;

	inter_tset->enable = 2;
	//inter_tset->set = 0b10000;

	volatile if_hw_mem_dbgu* dbgu_test = (if_hw_mem_dbgu*) DBGU;

	dbgu_test->interrupt_disable = 0xFFFFFFFF;
	dbgu_test->interrupt_enable = 1;

	//dbgu_write_async(17, "async writing!\r\n");
	//dbgu_async_write_flush();

	/* interrupt tst loop*/
	while (42) {
		asm volatile("nop":::);
		/*
		char c = get_recvbuff_head();
		for (int i = 0; i < 69; ++i) {
			printf("%c", c);
			worstdelayever(10);
		}
		dbgu_async_read_flush();*/
	}
	

/*
	print_banner();
	injection_trainer();
*/
	/* run stuff 
	print_banner();
	asm(".word 0x775f0074" : : : "memory");
	io_demo();
	*/

}

#pragma GCC pop_options
