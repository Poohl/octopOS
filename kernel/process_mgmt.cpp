extern "C" {
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

#define NUM_THREADS 16

typedef struct {
	uint id;
	int state;
	char name[8];
	cpu_context context;
} tcb;

uint current;

tcb processes[NUM_THREADS];
LoopQueue<uint, NUM_THREADS> tcb_free_q;

extern "C" void idle() {
	while (1)
		tight_powersave();
}

void exit(u32* hw_context) {
	// Don't call tis directly frem kernel... Why would you?
	// if (current.open_handles == 0)
	processes[current].state = DEAD;
	thread_swap_callback(hw_context);
}

void init_process_mgmt() {
	memset(processes, 0, sizeof(processes));	// clear tcb-array from ghosts
	current = 0;
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
}

int new_thread(const char* name, init_thread_state_args* args) {
	uint* id_p = tcb_free_q.pop();
	if (!id_p) return -1;
	int id = *id_p;
	cpu_context_init(&processes[id].context, args);
	processes[id].name[7] = 0;
	memcpy(&processes[id].name, name, 7);
	processes[id].id = id;
	processes[id].state = ALIVE;
	printf("created thread with id %x\n", id);
	printf("tcb queue fill:%x\r\n", tcb_free_q.get_full());
	return id;
}

int new_thread_raw(const char* name, cpu_context* init_state, bool may_be_sys) {
	if (!cpu_context_validate(init_state, may_be_sys)) return -2;
	uint* id_p = tcb_free_q.pop();;
	if (!id_p) return -1;
	int id = *id_p;
	memcpy(&processes[id].context, init_state, sizeof(cpu_context));
	processes[id].name[7] = 0;
	memcpy(&processes[id].name, name, 7);
	processes[id].state = ALIVE;
	return id;
}

void thread_swap_callback(u32* context) {
	uint next;
	for (next = (current + 1) % NUM_THREADS; next != current; next = (next + 1) % NUM_THREADS) {
		if (processes[next].state == ALIVE && next != 0)
			break;

		// get your chainsaw and your shotgun cause we're going on a zombie-hunt
		if (processes[next].state == ZOMBIE) {
			tcb_free_q.push(processes[next].id);
			processes[next].state = DEAD;
			printf("BOOM! -> %x\n", processes[next].id);
		}
	}
	if (next == current) {
		next = 0;
	}
	printf("Swap from slot %x to %x\r\n", current, next);
	swap(&processes[current].context, context, &processes[next].context);
	current = next;
}
