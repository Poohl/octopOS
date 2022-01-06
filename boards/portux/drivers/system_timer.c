

#include "system_timer.h"

#include "board.h"

#include "libs/hardware.h"

#define STATUS_PERIOD (1 << 0)
#define STATUS_ALARM (1 << 3)

static volatile system_timer* st = (system_timer*) SYSTEM_TIMER;

static u32 next_period;

static void set_period_and_restart(u16 period) {
	st->interval_mode = period;
	st->interrupt_enable = STATUS_PERIOD;
}

static void disable_interval_timer() {
	st->interrupt_disable = STATUS_PERIOD;
}

int set_timer_interval(uint period) {
	if (period > 0x10000) {
		return -1;
	} else if (period == 0) {
		disable_interval_timer();
	} else {
		set_period_and_restart(period);
	}
	return 0;
}

/*
 * most harware timers allow you to directly load
 * the current value, but apparently not this one
 * Problem with this workaround:
 * When the interrupt is already pending, nothing clears it.
 * And we probably can't fix that, because either we read
 * the status and clear all interrupts or we don't and don't.
 */
int set_next_timer_interval(uint delay) {
	if (delay > 0x10000) {
		return -1;
	} else {
		next_period = st->interval_mode | 0x10000;
		st->interval_mode = delay;
	}
}

bool timer_interrupt_callback() {
	if (next_period) {
		st->interval_mode = next_period;
		next_period = 0;
	}
	u32 status = st->status;
	if (status & STATUS_PERIOD) {
		timer_handler();
		return true;
	}
	return false;
}

