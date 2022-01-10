
#include "default.hpp"
#include "Callback.hpp"

class PeriodicTimer {
	void setCallback(Callback<>* callback);
	int setPeriod(uint period);
	int setNextDelay(uint delay);
};
