#include "default.h"

#include "drivers/uart.hpp"
#include "drivers/SysTick.hpp"
#include "kernel/init_kernel.hpp"

extern "C" {
#include "libs/printf.h"
#include "fluff/fluff.h"
#include "libs/hardware.h"
#include "drivers/vectors.h"
#include "drivers/cpu.h"
#include "board.h"
#include "kernel/syscalls.h"
#include "kernel/process_mgmt.h"
}

extern u32 __INIT_ARRAY_S__;
extern u32 __INIT_ARRAY_E__;

// needed to prevent gcc from optimizing c_entry out.

#pragma GCC push_options
#pragma GCC optimize ("O0")


extern "C" void abort() {
}

extern "C" void __cxa_pure_virtual() { while (1); }

void* memcpy(void* dest, const void* src, size_t n) {
	byte* _dest = (byte*) dest;
	byte* _src = (byte*) src;
	byte* _end = (byte*) _src + n;
	for (; _src <= _end; ++_src, ++_dest)
		*_dest = *_src;
	return dest;
}

Uart uart0;
SysTick sysTimer;

AsyncOutStream* debug_out_stream;
AsyncInStream* debug_in_stream;

extern "C" void systick_hand() {
	sysTimer.base_callback();
}

extern "C" void uart0_hand() {
	uart0.baseCallback();
}

/* MAIN */
void notmain(void) {
	return;
}

extern "C" void testfunc(BlockingOutStream* out) {
	while (1)
		out->write("func\r\n", 7);
}

/* MAIN */
extern "C"
void _start(void) {
	asm volatile(
		"add r0, #1"
	);
	for (u32* f = &__INIT_ARRAY_S__; f < &__INIT_ARRAY_E__; ((void_void_func_ptr) (*(f++)))());
	*((u32*) SYS_CTL_VTOR) = VECTORS_BASE;

	uart0.init((mmio_uart*) UART0_BASE);
	debug_out_stream = (AsyncOutStream*) &uart0;
	debug_in_stream = (AsyncInStream*) &uart0;
	sysTimer.init((mmio_systick*) SYSTICK_BASE);
	sysTimer.setPeriod(100000);
	init_stacks();
	init_vectors();
	uart0.BlockingOutStream::write("hello\r\n", 8);
	init_process_mgmt(&sysTimer);
	init_syscalls(NULL);
	init_thread_state_args args = default_init_thread_state_args;
	args.start = (void_void_func_ptr) &testfunc;
	args.is_sys = true;
	args.args[0] = (u32) ((BlockingOutStream*) &uart0);
	new_thread("tester", &args);
	sys_exit();
	printf("test");

	test();


	while (1);
			//asm volatile("wfi");
	
	print_banner();




}

#pragma GCC pop_options
