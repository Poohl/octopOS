
#ifndef TIMER_HPP
#define TIMER_HPP

#include "default.hpp"
#include "Callback.hpp"

class PeriodicTimer {
	public: 
		virtual void setCallback(Callback<>* callback);
		virtual int setPeriod(uint period);
};

class MultiAlarm {
	public:
		virtual void start(uint delay, Callback<>* expired);
};

#endif
