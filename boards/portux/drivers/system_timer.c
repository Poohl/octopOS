

#include "system_timer.h"

#include "board.h"

#include "libs/hardware.h"

#define STATUS_PERIOD (1 << 0)
#define STATUS_ALARM (1 << 3)

static volatile system_timer* st = (system_timer*) SYSTEM_TIMER;

void set_preiod_and_restart(u16 period) {
	st->interval_mode = period;
	st->interrupt_enable = STATUS_PERIOD;
}

int set_timer_interval(uint period) {
	if (period > 0x10000) {
		return -1;
	} else {
		set_preiod_and_restart(period);
	}
	return 0;
}

bool timer_interrupt_callback() {
	u32 status = st->status;
	if (status & STATUS_PERIOD) {
		timer_handler();
		return true;
	}
	return false;
}

