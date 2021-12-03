#include "default.h"

#include "board.h"

#include "drivers/uart.h"
#include "libs/printf.h"
#include "fluff/fluff.h"
#include "apps/injection_trainer.h"
#include "libs/hardware.h"

extern void init_stacks();

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")

/* MAIN */
void _start(void) {

	*((u32*) SYS_CTL_VTOR) = VECTORS_BASE;
	uart_init();

	init_stacks();

	set_timer_interval(1000000);

	print_banner();
	injection_trainer();


}

#pragma GCC pop_options