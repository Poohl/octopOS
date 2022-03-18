
#include "drivers/Timer.hpp"

typedef struct {
	volatile u32 control_and_status;
	volatile u32 period;
	volatile u32 value;
	volatile u32 calibration;
} mmio_systick;

class SysTick : public PeriodicTimer {
	private:
		Callback<>* callback;
		mmio_systick* base;
	public:
		void init(mmio_systick* b);
		void setCallback(Callback<>* c);
		int setPeriod(uint period);
	/* protected: */
		bool base_callback();
};
