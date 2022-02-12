
#ifndef PROCESS_MGMT_H_TYPES
#define PROCESS_MGMT_H_TYPES
struct _init_thread_state_args;
typedef struct _init_thread_state_args init_thread_state_args;
#endif

#ifndef PROCESS_MGMT_H
#define PROCESS_MGMT_H

#include "default.h"
#include "drivers/cpu.h"

struct _init_thread_state_args {
	void_void_func_ptr start;
	void_void_func_ptr exit;
	void* stack;
	u32 args[4];
	uint stack_size;
	bool is_sys;
	bool new_address_space;
};

void init_process_mgmt();

int new_thread(char* name, init_thread_state_args* args);

int new_thread_raw(char* name, cpu_context* init_state, bool may_be_sys);

void exit(u32* hw_context);

void thread_swap_callback(u32* context);

int block_current(u32* hw_context);

void unblock(uint id, u32 return_value);

void unblock_now(uint id, u32* hw_context, u32 return_value);

#define default_init_thread_state_args { \
	.start = NULL, \
	.exit = &sys_exit, \
	.stack = NULL, \
	.stack_size = 1 << 11, \
	.args = {0}, \
	.is_sys=false, \
	.new_address_space=true \
}

void sleep(u32* hw_context, u32 delay);

#endif
