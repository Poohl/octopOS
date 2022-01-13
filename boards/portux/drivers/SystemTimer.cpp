

#include "SystemTimer.hpp"

#define STATUS_PERIOD (1 << 0)
#define STATUS_ALARM (1 << 3)

SystemTimer::SystemTimer() : underlying(NULL), callback(NULL) {};

SystemTimer::SystemTimer(mmio_system_timer* b) : underlying(b), callback(NULL) {
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

int SystemTimer::setNextDelay(uint d) {
	if (!callback)
		return 0;
	delay = d;
	this->underlying->interval_mode = d;
	if (!period)
		this->underlying->interrupt_enable = STATUS_PERIOD;
	return 0;
}

bool SystemTimer::underlying_callback() {
	u32 status = this->underlying->status;
	if (status & STATUS_PERIOD) {
		this->callback->call();
		if (delay) {
			delay = 0;
			if (period)
				this->underlying->interval_mode = period;
			else
				this->underlying->interrupt_disable = STATUS_PERIOD;
		}
		return true;
	}
	return false;
}

