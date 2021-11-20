#include "default.h"

#include "board.h"

#include "drivers/uart.h"
#include "libs/printf.h"
#include "fluff/fluff.h"
#include "apps/demo.h"
#include "libs/hardware.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")

/* MAIN */
void _start(void) {

	*((u32*) SYS_CTL_VTOR) = VECTORS_BASE;
	uart_init();

	asm(".word 0xb787":::"memory");

	print_banner();
	io_demo();

}

#pragma GCC pop_options