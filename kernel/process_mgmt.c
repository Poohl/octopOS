
#include "drivers/cpu.h"
#include "process_mgmt.h"
#include "libs/printf.h"

#define DEAD	0
#define ALIVE	1
#define ZOMBIE	2

typedef struct {
	uint id;
	int state;
	char name[8];
	cpu_context context;
} tcb;

typedef struct {
	int buff[16];
	int available;
	int next;
} tcb_queue;

uint current = 0;

tcb processes[16];
tcb_queue tcbq;

void init_tcb_queue(tcb_queue* q) {
	for (int i = 0; i < 16; ++i)
		q->buff[i] = i;
	q->available = 16;
	q->next = 0;
}

int get_tcb_slot(tcb_queue* q){
	if (q->available > 0) {
		int ret = q->buff[q->next];
		q->available --;
		q->next = q->next + 1 & 0xf;
		return ret;
	} else {
		return -1;
	}
}

void release_tcb_slot(tcb_queue* q, int slt) {
	q->buff[q->next + q->available & 0xf] = slt;
	q->available++;
}

void idle() {
	while (1)
		tight_powersave();
}

void exit() {
	processes[current].state = ZOMBIE;
	idle();
}

void init_process_mgmt() {
	init_tcb_queue(&tcbq);
	current = 1;
	memset(processes, 0, sizeof(processes));	// clear tcb-array from ghosts
	
	init_thread_state_args idle_args = default_init_thread_state_args;
	idle_args.stack = &processes[0].context.registers;
	idle_args.start = &idle;
	idle_args.exit = &idle;
	idle_args.is_sys = true;
	idle_args.stack_size = sizeof(processes[0].context.registers);
	new_thread("idle", &idle_args);
}

int new_thread(char* name, init_thread_state_args* args) {
	int id = get_tcb_slot(&tcbq);
	if (id < 0) return -1;
	cpu_context_init(&processes[id].context, args);
	processes[id].name[7] = 0;
	memcpy(&processes[id].name, name, 7);
	processes[id].state = ALIVE;
	return id;
}

int new_thread_raw(char* name, cpu_context* init_state, bool may_be_sys) {
	if (!cpu_context_validate(init_state, may_be_sys)) return -2;
	int id = get_tcb_slot(&tcbq);
	if (id < 0) return -1;
	memcpy(&processes[id].context, init_state, sizeof(cpu_context));
	processes[id].name[7] = 0;
	memcpy(&processes[id].name, name, 7);
	processes[id].state = ALIVE;
	return id;
}

void thread_swap_callback(u32* context) {
	uint next;
	for (next = current + 1; next != current; next = next + 1 & 0xF) {
		if (processes[next].state == ALIVE && next != 0)
			break;

		// get your chainsaw and your shotgun cause we're going on a zombie-hunt
		if (processes[next].state == ZOMBIE) {
			release_tcb_slot(&tcbq, processes[next].id);
			processes[next].state = DEAD;
		}
	}
	if (next == current) {
		next = 0;
	}
	printf("Swap from slot %x to %x", current, next);
	swap(&processes[current].context, context, &processes[next].context);
	current = next;
}
