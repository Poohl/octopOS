
#include "default.h"

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
} system_timer;

void set_preiod_and_restart(u16 period);

bool timer_interrupt_callback();