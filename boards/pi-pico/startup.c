#include "default.h"

#include "board.h"

#include "drivers/uart.h"
#include "libs/printf.h"
#include "fluff/fluff.h"
#include "apps/injection_trainer.h"
#include "apps/demo.h"
#include "libs/hardware.h"
#include "vectors.h"
#include "test.h"

// needed to prevent gcc from optimizing c_entry out.

#pragma GCC push_options
#pragma GCC optimize ("O0")

void abort() {
}

void* memcpy(void* dest, const void* src, size_t n) {
	byte* _dest = (byte*) dest;
	byte* _src = (byte*) src;
	byte* _end = (byte*) _src + n;
	for (; _src <= _end; ++_src)
		*_src = *_dest;
}


/* MAIN */
void _start(void) {

	*((u32*) SYS_CTL_VTOR) = VECTORS_BASE;
	uart_init();

	init_stacks();
	init_vectors();

	testfunc();
	uart_write_async(27, "This is same async stuff\r\n");
	uart_async_write_flush();


	set_timer_interval(1000000);

	print_banner();
	injection_trainer();


}

#pragma GCC pop_options