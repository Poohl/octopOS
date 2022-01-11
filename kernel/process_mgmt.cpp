extern "C" {
#include "process_mgmt.h"
#include "drivers/cpu.h"
#include "process_mgmt.h"
#include "libs/printf.h"
#include "libs/hardware.h"
#include "syscalls.h"
}
#include "libs/loop_queue.hpp"
#define DEAD	0
#define ALIVE	1
#define ZOMBIE	2
#define BLOCKED	3

#define NUM_THREADS 16

typedef struct {
	uint id;
	int state;
	char name[8];
	cpu_context context;
} tcb;

uint current;
uint real_alive_threads;

tcb processes[NUM_THREADS];
LoopQueue<uint, NUM_THREADS> tcb_free_q;

extern "C" void idle() {
	set_timer_interval(0);
	while (1)
		tight_powersave();
}

void exit(u32* hw_context) {
	// Don't call tis directly frem kernel... Why would you?
	// if (current.open_handles == 0)
	processes[current].state = DEAD;
	if (--real_alive_threads == 1)
		set_timer_interval(0);
	thread_swap_callback(hw_context);
}

void init_process_mgmt() {
	current = 15;
	memset(processes, 0, sizeof(processes));	// clear tcb-array from ghosts
	for (int i = 0; i < NUM_THREADS; ++i) {
		tcb_free_q.push(i);
		processes[i].id = i;
	}
	init_thread_state_args idle_args = default_init_thread_state_args;
	idle_args.stack = &processes[0].context.registers;
	idle_args.start = &idle;
	idle_args.exit = &idle;
	idle_args.is_sys = true;
	idle_args.stack_size = sizeof(processes[0].context.registers);
	new_thread("idle", &idle_args);
	real_alive_threads = 0;
}

static int internal_new_thread_finalizer(const char* name, int id) {
	processes[id].name[7] = 0;
	memcpy(&processes[id].name, name, 7);
	processes[id].id = id;
	processes[id].state = ALIVE;
	printf("created thread with id %x\n", id);
	if (++real_alive_threads == 2)
		set_timer_interval(10000);
	return id;
}

extern "C"
int new_thread(const char* name, init_thread_state_args* args) {
	uint* id_p = tcb_free_q.pop();
	if (!id_p) return -1;
	int id = *id_p;
	if (!args->stack)
		args->stack = (void*) (PROCESS_STACKS - (id - 1) * 0x5000);
	cpu_context_init(&processes[id].context, args);
	return internal_new_thread_finalizer(name, id);
}

extern "C"
int new_thread_raw(const char* name, cpu_context* init_state, bool may_be_sys) {
	if (!cpu_context_validate(init_state, may_be_sys)) return -2;
	uint* id_p = tcb_free_q.pop();
	if (!id_p) return -1;
	int id = *id_p;
	memcpy(&processes[id].context, init_state, sizeof(cpu_context));
	return internal_new_thread_finalizer(name, id);
}

/**
 * Is called by the interrupt handler and passed the saved context at the time of interrupt
 * This context is consumed by the swap routine in cpu.c
 */
void thread_swap_callback(u32* context) {
	uint next;
	for (next = (current + 1) % NUM_THREADS; next != current; next = (next + 1) % NUM_THREADS) {
		if (processes[next].state == ALIVE && next != 0)
			break;
	}
	if (next == current && processes[current].state != ALIVE) {
		next = 0;
	}
	if (current != next) {
		printf("Swap from slot %x to %x\r\n", current, next);
		swap(&processes[current].context, context, &processes[next].context);
		current = next;
	} else {
		printf("Warning: swap ID on %x, alive_threads: %x\r\n", current, real_alive_threads);
	}
	//print_q(&tcbq);
}

uint block_current(u32* hw_context) {
	processes[current].state = BLOCKED;
	uint out = current;
	if (--real_alive_threads == 1)
		set_timer_interval(0);
	thread_swap_callback(hw_context);
	return out;
}

void unblock(uint id) {
	if (++real_alive_threads == 2)
		set_timer_interval(10000);
	processes[id].state = ALIVE;
}

void unblock_now(uint id, u32* hw_context) {
	unblock(id);
	swap(&processes[current].context, hw_context, &processes[id].context);
	current = id;
}

void unblock_fancy(uint id, u32* hw_context, u32* return_vals, uint return_count) {
	if (processes[id].state == ALIVE)
		return;
	if (return_vals)
		set_return_values(&processes[id].context, return_vals, return_count);
	if (hw_context)
		unblock_now(id, hw_context);
	else
		unblock(id);
}