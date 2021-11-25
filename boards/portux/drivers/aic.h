
#ifndef IRQ_H
#define IRQ_H

#include "default.h"

typedef void(*isr)(void);

typedef const u32 u32_ro;

typedef struct {
	u32 isr_mode[32];
	isr isr_target[32];
	u32_ro current_target;
	u32_ro fast_current_target;
	u32_ro status;
	u32_ro pending;
	u32_ro mask;
	u32_ro core_status;
	u32 _reserved1[2];
	u32 enable;
	u32 disable;
	u32 clear;
	u32 set;
	u32 signal_end;
	isr spurious_target;
	u32 debug;
	u32 _reserved2;
} aic;

#define AIC_MODE_PRIO_MASK 0b111
#define AIC_MODE_TRIGGER_LOW (0b00 << 5)
#define AIC_MODE_TRIGGER_RISING (0b01 << 5)
#define AIC_MODE_TRIGGER_HIGH (0b10 << 5)
#define AIC_MODE_TRIGGER_FALLING (0b11 << 5)

#endif