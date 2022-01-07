
#include "default.hpp"

class PeriodicTimer {
	void setCallback(void_void_func_ptr callback);
	int setPeriod(uint period);
	int setNextDelay(uint delay);
};
