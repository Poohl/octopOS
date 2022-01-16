
#include "default.hpp"
#include "PeriodBasedMultiAlarm.hpp"

bool alarmStorageComp(const AlarmStorage* a, const AlarmStorage* b) {
	return a->done_time <= b->done_time;
}

PeriodBasedMultiAlarm::PeriodBasedMultiAlarm() {};

void PeriodBasedMultiAlarm::init(PeriodicTimer* _base, uint _resolution, bool _powersave) {
	base = _base;
	resolution = _resolution;
	powersave = _powersave;
	period = 0;
	time = 0;
	next_period = ~0;
	base->setCallback(this);
	if (powersave)
		updateBase();
	else
		base->setPeriod(resolution);
}

void PeriodBasedMultiAlarm::call() {
	time += resolution;
	if (period && periodCallback && time >= next_period) {
		periodCallback->call();
		next_period += period;
	}
	for (AlarmStorage* a = alarms.peek(); a && a->done_time <= time; alarms.pop(), a = alarms.peek())
		if (a->done_time <= time)
			a->expired->call();
}

int PeriodBasedMultiAlarm::setResolution(uint res) {
	return base->setPeriod(resolution = res);
}

int PeriodBasedMultiAlarm::setPeriod(uint _period) {
	period = _period;
	return 0;
}

void PeriodBasedMultiAlarm::updateBase() {
	if (!powersave)
		return;
	if (period && periodCallback) {
		switch (alarms.get_full()) {
			case 0:
				base->setPeriod(period);
				break;
			default:
				base->setPeriod(resolution);
		}
	} else {
		switch (alarms.get_full()) {
			case 0:
				base->setPeriod(period);
				break;
			case 1:
				base->setPeriod(alarms.peek()->done_time - time);
				break;
			default:
				base->setPeriod(resolution);
		}
	}
}

void PeriodBasedMultiAlarm::setCallback(Callback<>* _callback) {
	periodCallback = _callback;
}

void PeriodBasedMultiAlarm::start(uint delay, Callback<>* expired) {
	AlarmStorage newAlarm = {
		.expired = expired,
		.done_time = time + delay
	};
	alarms.put(&newAlarm);
}