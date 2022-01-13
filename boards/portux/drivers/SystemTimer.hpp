
#include "default.hpp"
#include "drivers/Timer.hpp"

typedef struct {
	volatile u32 control;
	volatile u32 interval_mode;
	volatile u32 watchdog_mode;
	volatile u32 realtime_mode;
	volatile u32 status;
	volatile u32 interrupt_enable;
	volatile u32 interrupt_disable;
	volatile u32 interrupt_mask;
	volatile u32 realtime_alarm;
	volatile u32 realtime;
} mmio_system_timer;

class SystemTimer : public PeriodicTimer {
	private:
		volatile mmio_system_timer* underlying;
		uint period;
		bool delay;
		Callback<>* callback;
	public:
		SystemTimer();
		SystemTimer(mmio_system_timer* b);
		void setCallback(Callback<>* c);
		int setPeriod(uint period);
		int setNextDelay(uint delay);
	/* protected: */
		bool underlying_callback();
};