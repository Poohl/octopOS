#include "drivers/dbgu.hpp"
#include "drivers/SystemTimer.hpp"
#include "drivers/PeriodBasedMultiAlarm.hpp"
#include "kernel/init_process_mgmt.hpp"
//#include <new>

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

void _exit(int x) {};
void* _sbrk(intptr_t s) {return NULL;};
int _kill(int p, int s) {return 0;};
int _getpid() {return 0;};
}

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")


DebugUnit dbgu;
SystemTimer timer;
PeriodBasedMultiAlarm alarmTimer;

AsyncOutStream* debug_out_stream = &dbgu;
AsyncInStream* debug_in_stream = &dbgu;

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

static void stupid_spinner() {
	while (1) {
		printf("hEy, LisTEn!\r\n");
		for (uint i = 0; i < 10000000; ++i)
			asm volatile("nop" : : : "memory");
	}
}

static void init_thread() {
	while (1) {
		printf("lets go->");
		sys_debug_put_char(0, '#');
		printf("WORKING\r\n");
		sys_sleep(100000);
		printf("Good morning\r\n");
		for (int i = 0; i < 100000; ++i)
			asm volatile("" : : : "memory");
	}
}

static PeriodicTimer* timer_p = &timer;

/* MAIN */
extern "C"
void c_entry(void) {
	/* init  */
	for (u32* f = &__INIT_ARRAY_S__; f < &__INIT_ARRAY_E__; ((void_void_func_ptr) (*(f++)))());
	dbgu.init((mmio_dbgu*) DBGU);
	printf("debug io works\n\r");
	timer.init((mmio_system_timer*) SYSTEM_TIMER);

	timer_p->setPeriod(10);

	alarmTimer.init(&timer, 1000, false);
	//new (&alarmTimer) PeriodBasedMultiAlarm(&timer, 1000, false);
	init_stacks();
	init_vector_handling();

	/* interrups */
	u32 buff;

	asm("mrs %0, cpsr" :  "=r" (buff) : : );
	printf_cpsr(buff);
	init_process_mgmt((PeriodicTimer*) &alarmTimer);
	init_syscalls();

	enable_interrupts();

	asm("mrs %0, cpsr" :  "=r" (buff) : : );
	printf_cpsr(buff);

	set_interrupt_handler(1, &system_interrupt_hand, 0, 0);

	/* interrupt enable @ aic (2)*/
	volatile aic* inter_tset = (aic*) AIC;

	inter_tset->enable = 2;
	//inter_tset->set = 0b10000;

	/* interrupt tst loop*/

	init_thread_state_args initt = default_init_thread_state_args;
	initt.start = &init_thread;
	new_thread("init", &initt);

	//init_thread_state_args spint = default_init_thread_state_args;
	//spint.start = &stupid_spinner;
	//new_thread("spin", &spint);

	sys_exit();
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
