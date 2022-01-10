#include "drivers/dbgu.hpp"
#include "drivers/SystemTimer.hpp"

extern "C" {
#include "libs/hardware.h"
#include "drivers/vectors.h"
#include "fluff/fluff.h"
#include "libs/printf.h"
#include "drivers/aic.h"
#include "board.h"
#include "drivers/cpu.h"
#include "kernel/process_mgmt.h"
#include "kernel/syscalls.h"
}

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")


DebugUnit dbgu;
SystemTimer timer;

extern u32 __INIT_ARRAY_S__;
extern u32 __INIT_ARRAY_E__;

class NothingCallback : public Callback<> {
	public:
		void call() {};
};

extern "C" {

	void do_nothing() {};

	void __cxa_pure_virtual() { while (1); }

	extern void init_stacks();

	extern void system_interrupt_hand();

	bool dbgu_interupt_callback() {
		return dbgu.baseCallback();
	}
	bool timer_interrupt_callback() {
		return timer.underlying_callback();
	}

	int debug_put_char(char c) {
		return dbgu.write_byte(c);
	}
	int debug_get_char() {
		return dbgu.read_byte();
	}

	sequence_io_status debug_write(uint len, const byte* data) {
		return dbgu.BlockingOutStream::write(data, len);
	}
	
	sequence_io_status debug_read(uint len, byte* buff) {
		return dbgu.BlockingInStream::read(buff, len);
	}

	int set_timer_interval(uint period) {
		return timer.setPeriod(period);
	}

}

static void init_thread() {
	while (1) {
		printf("Working!\r\n");
		for (int i = 0; i < 100000; ++i)
			asm volatile("" : : : "memory");
	}
}

/* MAIN */
extern "C"
void c_entry(void) {
	/* init  */
	for (u32* f = &__INIT_ARRAY_S__; f < &__INIT_ARRAY_E__; ((void_void_func_ptr) (*(f++)))());
	dbgu = DebugUnit((mmio_dbgu*) DBGU);
	timer = SystemTimer((mmio_system_timer*) SYSTEM_TIMER);
	NothingCallback no_callback;
	timer.setCallback(&no_callback);
	init_stacks();
	init_vector_handling();

	

	/* interrups */
	u32 buff;

	asm("mrs %0, cpsr" :  "=r" (buff) : : );
	printf_cpsr(buff);
	init_process_mgmt();
	init_syscalls();

	enable_interrupts();

	asm("mrs %0, cpsr" :  "=r" (buff) : : );
	printf_cpsr(buff);

	set_interrupt_handler(1, &system_interrupt_hand, 0, 0);

	/* system clock */
	/*    SET INTERVAL HERE
						||
						||
					   \  /
					    \/			*/
	timer.setPeriod(10000);

	/* interrupt enable @ aic (2)*/
	volatile aic* inter_tset = (aic*) AIC;

	inter_tset->enable = 2;
	//inter_tset->set = 0b10000;

	/* interrupt tst loop*/

	init_thread_state_args initt = default_init_thread_state_args;
	initt.start = &init_thread;
	new_thread("init", &initt);

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