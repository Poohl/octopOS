
#ifndef PROCESS_MGMT_H
#define PROCESS_MGMT_H

// don't touch this. I don't know why it works.
// reason it's there is the circular include CPU<->process_mgmt
#ifdef CPU_H
typedef struct _ccccccc cpu_context;
#endif

#include "default.h"
#include "drivers/cpu.h"

// WORDS CANNOT DESCRIBE HOW MUCH I HATE THE GCC PIPLINE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
typedef struct _init_thread_state_args {
	void_void_func_ptr start;
	void_void_func_ptr exit;
	void* stack;
	u32 args[4];
	uint stack_size;
	bool is_sys;
}  init_thread_state_args;

void init_process_mgmt();

int new_thread(char* name, init_thread_state_args* args);

int new_thread_raw(char* name, cpu_context* init_state, bool may_be_sys);

void exit(u32* hw_context);

void thread_swap_callback(u32* context);

#define default_init_thread_state_args { \
	.start = NULL, \
	.exit = &sys_exit, \
	.stack = NULL, \
	.stack_size = 1 << 11, \
	.args = {0}, \
	.is_sys=false \
}

#endif
