

#include "SysTick.hpp"
extern "C" {
#include "board.h"
}
#define STATUS_LOOPED (1 << 16)
#define CONTROL_CLKSOURCE_CPU (1 << 2)
#define CONTROL_INT (1 << 1)
#define CONTROL_ENABLE (1 << 0)
#define VALUE_MAX 0x00FFFFFF

void SysTick::init(mmio_systick* b) {
	base = b;
	callback = NULL;
	base->control_and_status = CONTROL_CLKSOURCE_CPU;
	base->control_and_status;
}

void SysTick::setCallback(Callback<>* c) {
	if (!c)
		*hw_clear_alias(&base->control_and_status) = CONTROL_INT;
	this->callback = c;
	if (c && base->period)
		*hw_set_alias(&base->control_and_status) = CONTROL_INT | CONTROL_ENABLE;
}

int SysTick::setPeriod(uint p) {
	if (p >= 0x1000000)
		return -1;
	if (p == 0)
		*hw_clear_alias(&base->control_and_status) = CONTROL_INT | CONTROL_ENABLE;
	base->period = p;
	base->value = p;
	if (p && callback)
		*hw_set_alias(&base->control_and_status) = CONTROL_INT | CONTROL_ENABLE;
	return 0;
}

bool SysTick::base_callback() {
	if (base->control_and_status & STATUS_LOOPED) {
		callback->call();
		return true;
	}
	return false;
}
