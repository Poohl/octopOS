
#ifndef CPU_H
#define CPU_H

#include "default.h"
#include "board.h"
#include "kernel/process_mgmt.h"

void printf_cpsr(u32 cpsr);

// this _ccccccc thing is very important (see kernel/process_mgmt.h)
typedef struct _ccccccc {
	u32 registers[13];
	u32 pc;
	u32 sp;
	u32 lr;
	u32 cpsr;
} cpu_context;

void cpu_context_init(cpu_context* dest, const init_thread_state_args* src);

bool cpu_context_validate(cpu_context* dest, bool may_be_sys);

void tight_powersave();

void swap(cpu_context* curr, u32* curr_context, cpu_context* next);

#endif
