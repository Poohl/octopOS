
#include "default.h"
#include "process_mgmt.h"
#include "drivers/cpu.h"

typedef struct {
	uint id;
	bool alive;
	bool ready;
	thread_args startup; // this is optinal
	cpu_context context;
} tcb;

uint id_allocator = 0;

uint running_slot = 0;

tcb processes[16];

int exit() {
	processes[running_slot].alive = false; // this is a horrible race condition.
	// wait for beeing descheduled
	// we don't just call idle, because that might do misc stuff we don't want to die
	while (1)
		tight_powersave();
}

void idle() {
	// mine bitcoin
	while (1)
		tight_powersave();
}

void init_thread(tcb* self, thread_args* args) {
	self->startup = *args;
	self->id = ++id_allocator;
	init_context(&self->context, args);
	self->alive = true;
	self->ready = true;
}

int new_thread(thread_args* args) {
	uint free_slot;
	for (free_slot = 0; free_slot <= ARR_LEN(processes) && processes[free_slot].alive; ++free_slot);
	if (free_slot >= 16)
		return -1;
	init_thread(&processes[free_slot], args);
}

void init_process_mgmt() {
	running_slot = 0;
	memset(processes, 0, sizeof(processes));
	thread_args idle_args = {
		.start = &idle,
		.exit = &idle,
		.stack = NULL,
		.args = {0},
		.is_sys = true
	};
	init_thread(&processes[0], &idle_args);
}

void thread_swap_callback(u32* context) {
	uint next;
	for (next = running_slot + 1; next != running_slot; next = next + 1 & 0xF) {
		if (processes[next].alive && next != 0)
			break;
	}
	if (next == running_slot) {
		next = 0;
	}
	swap(&processes[running_slot], context, &processes[next]);
	running_slot = next;
}
