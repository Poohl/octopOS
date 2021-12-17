
#include "default.h"
#include "board.h"
#include "cpu.h"

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
	while (1) {
		asm volatile(".word 0xe320f003");	// MINE BITCOINS HERE
	}
}

void *memset(void *s, int c, size_t n) {
	for (size_t i = 0; i < n; ++i)
		((byte*) s)[i] = c;
	return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
	for (size_t i = 0; i < n; ++i)
		((byte*) dest)[i] = ((byte*) src)[i];
	return dest;
}

void init_thread(tcb* dest, uint id, void_void_func_ptr start, u32* stack, void_void_func_ptr exit, bool is_sys, u32 arg) {
	memset(dest, sizeof(*dest), 0);
	dest->context.registers[0] = arg;
	dest->id = id;
	dest->context.pc = start;
	dest->context.lr = exit;
	dest->context.sp = stack;
	dest->state = ALIVE;
	u32 tmp;
	asm ("mrs %0, cpsr" : "=r" (tmp) : :);
	tmp = (tmp & ~0x1F) | (is_sys ? MODE_SYS : MODE_USR);
	dest->context.cpsr = tmp;
}

int new_thread(void_void_func_ptr func, bool is_sys, int arg) {
	int id;
	int stack; // TODO: find a clever way to handle

	if (id = get_tcb_slot(&tcbq) < 0) return -1;
	init_thread(&processes[id], id, func, stack, &exit, is_sys, arg);
	return id;
}

void exit() {
	processes[current].state = ZOMBIE;
	idle();
}

void cpu_init() {
	init_tcb_queue(&tcbq);
	current = 1;
	memset(processes, 0, sizeof(processes));	// clear tcb-array from ghosts
	
	// reserve slot in queue
	tcbq.available--;
	tcbq.next++;
	
	init_thread(&processes[0], 0, &idle, &processes[0].context.registers[12], &idle, true, 0);
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
	swap(&processes[current], context, &processes[next]);
	current = next;
}


void swap(tcb* curr, u32* curr_context, tcb* next) {
	// save r0-12, lr as pc
	memcpy(&curr->context, curr_context, sizeof(curr->context.registers) + sizeof(u32));
	
	volatile u32 e = (next->context.cpsr) ,f;
	//save cpsr and move in new one, hw copied/banked it to spsr
	asm(
		"mrs %0, spsr\n"
		: "=r" (f) : : );
	curr->context.cpsr = f;
	asm volatile(
		"msr spsr, %0\n"
		: : "r" (e)	:
	);
	
	//save sp & lr, move in new ones
	u32 a,b,c = next->context.sp, d = next->context.lr;
	 
	asm volatile(
		"MRS r3,CPSR\n"
		"BIC r4,r3,#0x1F\n"
		"ORR r4,r4,#0b11111\n"
		"MSR cpsr, r4\n"
		"mov %0, sp\n"
		"mov %1, lr\n"
		"mov sp, %2\n"
		"mov lr, %3\n"
		"MSR cpsr, r3\n"
		: "+r" (a), "+r" (b)
		: "r" (c), "r" (d) 
		: "r3", "r4", "memory", "sp", "lr", "cc"
	);
	curr->context.sp = a;
	curr->context.lr = b;

	// move new context to stack, to be restored upon return
	memcpy(curr_context, next->context.registers, sizeof(next->context.registers) + sizeof(u32));
	// to create new threads, just zero this out.
	// or be friendly and put a call to exit into the lr
}