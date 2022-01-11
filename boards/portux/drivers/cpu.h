
#ifndef CPU_H_TYPES
#define CPU_H_TYPES
struct _cpu_context;
typedef struct _cpu_context cpu_context;
#endif

#ifndef CPU_H
#define CPU_H

#include "default.h"
#include "board.h"
#include "kernel/process_mgmt.h"

void printf_cpsr(u32 cpsr);

struct _cpu_context {
	u32 registers[13];
	u32 pc;
	u32 sp;
	u32 lr;
	u32 cpsr;
};

void cpu_context_init(cpu_context* dest, const struct _init_thread_state_args* src);

bool cpu_context_validate(cpu_context* dest, bool may_be_sys);

void tight_powersave();

void swap(cpu_context* curr, u32* curr_context, cpu_context* next);

void set_return_values(cpu_context* dest, u32* values, uint count);

#endif
