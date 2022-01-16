

#include "SystemTimer.hpp"

#define STATUS_PERIOD (1 << 0)
#define STATUS_ALARM (1 << 3)

void SystemTimer::init(mmio_system_timer* b) {
	underlying = b;
	callback = NULL;
	this->underlying->interrupt_disable = 0xFFFFFFFF;
}

void SystemTimer::setCallback(Callback<>* c) {
	if (!c)
		this->underlying->interrupt_disable = STATUS_PERIOD;
	this->callback = c;
	if (c && period)
		this->underlying->interrupt_enable = STATUS_PERIOD;
}

int SystemTimer::setPeriod(uint p) {
	if (p >= 0x10000)
		return -1;
	if (p == 0)
		this->underlying->interrupt_disable = STATUS_PERIOD;
	period = p;
	this->underlying->interval_mode = p;
	if (p && callback)
		this->underlying->interrupt_enable = STATUS_PERIOD;
	return 0;
}

bool SystemTimer::underlying_callback() {
	u32 status = this->underlying->status;
	if (status & STATUS_PERIOD) {
		this->callback->call();
		return true;
	}
	return false;
}

