#ifndef PERIOD_BASED_MULTI_ALARM_HPP
#define PERIOD_BASED_MULTI_ALARM_HPP

#include "default.hpp"
#include "Callback.hpp"
#include "Timer.hpp"
#include "libs/loop_queue.hpp"


typedef struct {
	Callback<>* expired;
	uint done_time;
} AlarmStorage;

class PeriodBasedMultiAlarm : public MultiAlarm, public PeriodicTimer, private Callback<>  {
	private:
		PeriodBasedMultiAlarm(const PeriodBasedMultiAlarm&);
		PeriodicTimer* base;
		Callback<>* periodCallback = NULL;
		uint resolution = 0;
		uint period = 0;
		uint next_period = ~0;
		uint time = 0; // not actually usable as "time", just runs when needed
		LoopQueue<AlarmStorage, 32> alarms;
		bool powersave;
		void call();
		void updateBase();
	public:
		PeriodBasedMultiAlarm();
		void init(PeriodicTimer* _base, uint _resolution, bool _powersave);
		int setResolution(uint res);
		int setPeriod(uint period);
		void setCallback(Callback<>* _callback);
		void start(uint delay, Callback<>* expired);
};

#endif
