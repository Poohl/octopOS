
#ifndef CPU_H
#define CPU_H

#include "default.h"
#include "board.h"

typedef struct {
	u32 registers[13];
	u32 pc;
	u32 sp;
	u32 lr;
	u32 cpsr;
} cpu_context;

#define DEAD	0
#define ALIVE	1
#define ZOMBIE	2

typedef struct {
	uint id;
	int state;
	cpu_context context;
} tcb;

typedef struct {
	int buff[16];
	int available;
	int next;
} tcb_queue;

void init_tcb_queue(tcb_queue* q);
int get_tcb_slot(tcb_queue* q);
void release_tcb_slot(tcb_queue* q, int slt);
int new_thread(void_void_func_ptr func, bool is_sys, int arg);
void init_thread(tcb* dest, uint id, void_void_func_ptr start, u32* stack, void_void_func_ptr exit, bool is_sys, u32 arg);

void exit();

void cpu_init();

void thread_swap_callback(u32* context);

void swap(tcb* curr, u32* curr_context, tcb* next);

#endif
