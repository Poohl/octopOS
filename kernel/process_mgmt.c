
#include "drivers/cpu.h"
#include "process_mgmt.h"
#include "libs/printf.h"
#include "syscalls.h"
#include "config.h"
#include "drivers/mmu.h"

#define DEAD	0
#define ALIVE	1
#define ZOMBIE	2
#define BLOCKED	3

typedef struct {
	uint id;
	int state;
	char name[8];
	u64 sleepy;
	uint address_space;
	cpu_context context;
	uint children;
	uint parent;
} tcb;

typedef struct {
	int buff[16];
	int available;
	int next;
} tcb_queue;

uint current;

uint real_alive_threads;
u64 keep_interrupts_enabled = 0;

tcb processes[16];
tcb_queue tcbq;

void init_tcb_queue(tcb_queue* q) {
	for (int i = 0; i < 16; ++i)
		q->buff[i] = i;
	q->available = 15;
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
	if (real_alive_threads < 2 && keep_interrupts_enabled < get_system_time())
		set_timer_interval(0);
	while (1)
		tight_powersave();
}

void exit(u32* hw_context) {
	// Don't call this directly from kernel... Why would you?

	// we can only release a process if it has no children,
	// as it might be owner of the address space.
	// we can only release the address_space if we're actually its owner.
	if (processes[current].children > 0) {
		processes[current].state = ZOMBIE;
	} else {
		processes[current].state = DEAD;
		if (processes[current].parent) {
			processes[processes[current].parent].children -= 1;
		} else {
			release_address_space(processes[current].address_space);
		}
		release_tcb_slot(&tcbq, current);
	}
	if (--real_alive_threads == 1 && keep_interrupts_enabled < get_system_time())
		set_timer_interval(0);
	thread_swap_callback(hw_context);
}

void init_process_mgmt() {
	init_tcb_queue(&tcbq);
	current = 15;
	memset(processes, 0, sizeof(processes));	// clear tcb-array from ghosts
	
	init_thread_state_args idle_args = default_init_thread_state_args;
	idle_args.stack = &processes[0].context.registers;
	idle_args.start = &idle;
	idle_args.exit = &idle;
	idle_args.is_sys = true;
	idle_args.stack_size = sizeof(processes[0].context.registers);
	new_thread("idle", &idle_args);
	real_alive_threads = 0;
}

static int internal_new_thread_finalizer(char* name, int id) {
	processes[id].name[7] = 0;
	memcpy(&processes[id].name, name, 7);
	processes[id].id = id;
	processes[id].state = ALIVE;
	processes[id].sleepy = 0;
	processes[id].children = 0;
	printf("created thread with id %x\n", id);
	if (++real_alive_threads == 2)
		set_timer_interval(SCHEDULER_TIMESLICE);
	return id;
}

int new_thread(char* name, init_thread_state_args* args) {
	int id = get_tcb_slot(&tcbq);
	if (id < 0) return -1;
	if (!args->stack)
		args->stack = (void*) (PROCESS_STACKS);// - (id - 1) * 0x100000);
	if (args->new_address_space) {
		processes[id].address_space = get_address_space();
		if (processes[id].address_space < 0) {
			release_tcb_slot(&tcbq, id);
			return -1;
		}
		printf("new ASP\r\n");
		set_ap(processes[id].address_space, PROCESS_STACKS, MEGABYTE, PROCESS_STACKS + (id - 1) * MEGABYTE, AP_RW_RW);
		processes[id].parent = 0;
	} else {
		processes[id].address_space = processes[current].address_space;
		args->stack = (void*) (PROCESS_STACKS + id * 64 * KILOBYTE);
		processes[current].children += 1;
		processes[id].parent = current;
	}
	cpu_context_init(&processes[id].context, args);
	return internal_new_thread_finalizer(name, id);
}

// this one is not yet updated for the children + address-space thing.
int new_thread_raw(char* name, cpu_context* init_state, bool may_be_sys) {
	return -1;
	if (!cpu_context_validate(init_state, may_be_sys)) return -2;
	int id = get_tcb_slot(&tcbq);
	if (id < 0) return -1;
	memcpy(&processes[id].context, init_state, sizeof(cpu_context));
	return internal_new_thread_finalizer(name, id);
}

/**
 * Is called by the interrupt handler and passed the saved context at the time of interrupt
 * This context is consumed by the swap routine in cpu.c
 */
void thread_swap_callback(u32* context) {
	uint next;
	for (next = (current + 1) & 0xF; next != current; next = next + 1 & 0xF) {
		if (processes[next].state == ALIVE &&
				processes[next].sleepy < get_system_time() &&
				next != 0)
			break;

		// get your chainsaw and your shotgun cause we're going on a zombie-hunt
		// we haven't upgraded to the recursive shotgun yet.
		if (processes[next].state == ZOMBIE && processes[next].children == 0) {
			release_tcb_slot(&tcbq, processes[next].id);
			processes[next].state = DEAD;
			if (processes[next].parent)
				processes[processes[next].parent].children -= 1;
			else
				release_address_space(processes[next].address_space);
			printf("BOOM! -> %x\n", processes[next].id);
		}
	}
	if (next == current && (processes[current].state != ALIVE || processes[next].sleepy >= get_system_time())) {
		next = 0;
	}
	printf("Swap from slot %x to %x (%x total)\r\n", current, next, real_alive_threads);
	swap_address_space(processes[current].address_space, processes[next].address_space);
	swap(&processes[current].context, context, &processes[next].context);
	current = next;
	//print_q(&tcbq);
}

int block_current(u32* hw_context) {
	processes[current].state = BLOCKED;
	int out = current;
	if (--real_alive_threads == 1 && keep_interrupts_enabled < get_system_time())
		set_timer_interval(0);
	thread_swap_callback(hw_context);
	return out;
}

void unblock(uint id, u32 return_value) {
	set_return_values(&processes[id].context, &return_value, 1);
	if (++real_alive_threads == 2)
		set_timer_interval(SCHEDULER_TIMESLICE);
	processes[id].state = ALIVE;
}

void unblock_now(uint id, u32* hw_context, u32 return_value) {
	unblock(id, return_value);
	swap(&processes[current].context, hw_context, &processes[id].context);
	current = id;
}

void sleep(u32* hw_context, u32 delay) {
	processes[current].sleepy = get_system_time() + delay;
	if (processes[current].sleepy > keep_interrupts_enabled)
		keep_interrupts_enabled = processes[current].sleepy;
	set_timer_interval(SCHEDULER_TIMESLICE);
	thread_swap_callback(hw_context);
}

void print_q(tcb_queue* q) {
	printf("### Q:\n");
	printf("av: %x\n",q->available);
	for (int i = 0; i < 16; ++i)
		printf("%x, ", q->buff[i]);
	printf("\nnx: %x\n",q->next);
	printf("######\n");
}