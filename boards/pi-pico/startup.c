#include "drivers/uart.h"
#include "libs/printf.h"
#include "fluff/fluff.h"
#include "apps/demo.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")

typedef void(*isr)(); 

static void dontCare() {
}

void _start(void);

isr vectors[] __attribute__((section(".vectors"))) = {
	[0 ... 16+32] = &dontCare,
	[0] = (isr) 0x20001000,
	[1] = &_start
};

/* MAIN */
void _start(void) {

	uart_init();

	print_banner();
	io_demo();

}

#pragma GCC pop_options