
#ifndef IRQ_H
#define IRQ_H

#include "default.h"

typedef void(*isr)(void);

typedef const u32 u32_ro;

typedef struct {
	volatile u32 isr_mode[32];
	volatile isr isr_target[32];
	volatile u32_ro current_target;
	volatile u32_ro fast_current_target;
	volatile u32_ro status;
	volatile u32_ro pending;
	volatile u32_ro mask;
	volatile u32_ro core_status;
	volatile u32 _reserved1[2];
	volatile u32 enable;
	volatile u32 disable;
	volatile u32 clear;
	volatile u32 set;
	volatile u32 signal_end;
	volatile isr spurious_target;
	volatile u32 debug;
	volatile u32 _reserved2;
} aic;

#define AIC_MODE_PRIO_MASK 0b111
#define AIC_MODE_TRIGGER_LOW (0b00 << 5)
#define AIC_MODE_TRIGGER_RISING (0b01 << 5)
#define AIC_MODE_TRIGGER_HIGH (0b10 << 5)
#define AIC_MODE_TRIGGER_FALLING (0b11 << 5)

#define acknowledge_interrupt() \
	((aic*) (AIC))->signal_end = 1

#endif