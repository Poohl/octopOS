
#ifndef CPU_H_TYPES
#define CPU_H_TYPES
struct _cpu_context;
typedef struct _cpu_context cpu_context;
struct _hw_cpu_context;
typedef struct _hw_cpu_context hw_cpu_context;
#endif

#ifndef CPU_H
#define CPU_H

#include "default.h"
#include "board.h"
#include "kernel/process_mgmt.h"

struct _hw_cpu_context {
	u32 low_registers[4];
	u32 ip_register;
	u32 lr;
	u32 pc;
	u32 xpsr;
};

struct _cpu_context {
	u32 low_registers[4];
	u32 ip_register;
	u32 lr;
	u32 pc;
	u32 xpsr;
	u32 registers[8];
	u32 return_const;
	u32 sp;
	u32 control;
	u32 primask;
};

void cpu_context_init(cpu_context* dest, const struct _init_thread_state_args* src);

bool cpu_context_validate(cpu_context* dest, bool may_be_sys);

void tight_powersave();

void isr_return();

void call_for_hw_context(void_void_func_ptr func, hw_cpu_context* context);

void swap(cpu_context* curr, u32* _ignore, cpu_context* next);

void set_return_values(cpu_context* dest, u32* values, uint count);

#endif