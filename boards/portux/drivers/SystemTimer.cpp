

#include "SystemTimer.hpp"

#define STATUS_PERIOD (1 << 0)
#define STATUS_ALARM (1 << 3)

SystemTimer::SystemTimer() : underlying(NULL), callback(NULL) {};

SystemTimer::SystemTimer(mmio_system_timer* b) : underlying(b), callback(NULL) {
	this->underlying->interrupt_disable = 0xFFFFFFFF;
}

void SystemTimer::setCallback(Callback<>* c) {
	if (c) {
		this->callback = c;
		this->underlying->interrupt_enable = STATUS_PERIOD;
	} else {
		this->underlying->interrupt_disable = STATUS_PERIOD;
		this->callback = c;
	}
}

int SystemTimer::setPeriod(uint period) {
	if (period == 0) {
		this->underlying->interrupt_disable = STATUS_PERIOD;
	} else if (period <= 0x10000) {
		this->underlying->interval_mode = period;
		this->underlying->interrupt_enable = STATUS_PERIOD;
	} else
		return -1;
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

