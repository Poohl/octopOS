

#include "systick.h"
#include "default.h"
#include "board.h"
#include "libs/hardware.h"

static volatile systick* _systick = (systick*) SYSTICK;

#define STATUS_LOOPED (1 << 16)
#define CONTROL_CLKSOURCE_CPU (1 << 2)
#define CONTROL_INT (1 << 1)
#define CONTROL_ENABLE (1 << 0)
#define VALUE_MAX 0x00FFFFFF

int set_timer_interval(uint period) {
	if (period > VALUE_MAX) {
		return -1;
	} else {
		_systick->control_and_status &= ~CONTROL_INT;
		_systick->period = period;
		_systick->value = 0;
		_systick->control_and_status |= CONTROL_INT | CONTROL_ENABLE | CONTROL_CLKSOURCE_CPU;
		return 1;
	}
}