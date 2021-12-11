
#ifndef CPU_H
#define CPU_H

#include "default.h"
#include "board.h"

typedef struct {
	u32 registers[14];
	u32 pc;
	u32 sp;
	u32 lr;
	u32 cpsr;
} cpu_context;

typedef struct {
	uint id;
	bool alive;
	cpu_context context;
} tcb;

void init_thread(tcb* dest, void_void_func_ptr start, u32* stack, void_void_func_ptr exit, bool is_sys);

void cpu_init();

void thread_swap_callback(u32* context);

void swap(tcb* curr, u32* curr_context, tcb* next);

#endif
