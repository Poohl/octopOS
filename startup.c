#include "memory-map.h"
#include "drivers/dbgu.h"
#include "libs/printf.h"
#include "fluff/fluff.h"
#include "apps/demo.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")

typedef void(*isr)(); 

static void dontCare() {
}

void c_entry(void);

isr vectors[] __attribute__((section(".vectors"))) = {
	[0 ... 16+32] = &dontCare,
	[0] = (isr) 0x20000000,
	[1] = &c_entry
};

/* MAIN */
void c_entry(void) {
	/* init driver */
	dbgu_init();

	/* run stuff */
	print_banner();
	io_demo();

}

#pragma GCC pop_options