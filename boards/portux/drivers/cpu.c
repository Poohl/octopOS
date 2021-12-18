
#include "default.h"
#include "board.h"
#include "cpu.h"
#include "libs/printf.h"

uint current = 0;

tcb processes[16];
tcb_queue tcbq;

void printf_cpsr(u32 cpsr) {
	const char* mode_str[] = {
		"USR",
		"FIQ",
		"IRQ",
		"SVC",
		"ERR",
		"ERR",
		"ERR",
		"ABT",
		"ERR",
		"ERR",
		"ERR",
		"UND",
		"ERR",
		"ERR",
		"ERR",
		"SYS"
	};
	printf(
			"%c%c%c%c %c??%c ???? ???? ??%c%c %c%c%c# %s\r\n", 
			(cpsr & CPSR_FLAG_NEGATIVE        ) ? 'N' : ' ',
			(cpsr & CPSR_FLAG_ZERO            ) ? 'Z' : ' ',
			(cpsr & CPSR_FLAG_CARRY           ) ? 'C' : ' ',
			(cpsr & CPSR_FLAG_OVERFLOW        ) ? 'V' : ' ',
			
			(cpsr & CPSR_FLAG_SATURATION      ) ? 'Q' : ' ',
			(cpsr & CPSR_FLAG_JAZELLE         ) ? 'J' : ' ',
			
			(cpsr & CPSR_FLAG_BIG_ENDIAN      ) ? 'E' : ' ',
			(cpsr & CPSR_FLAG_ASYNC_ABORT_MASK) ? 'A' : ' ',
			
			(cpsr & CPSR_FLAG_IRQ             ) ? 'I' : ' ',
			(cpsr & CPSR_FLAG_FIRQ            ) ? 'F' : ' ',
			(cpsr & CPSR_FLAG_THUMB           ) ? 'T' : ' ',
			
			(cpsr & 0x10) ? mode_str[cpsr & 0xF] : "ERR"
	);
}

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
	// clear mode & interrupt mask
	tmp &= ~(CPSR_FLAG_MODE_MASK | CPSR_FLAG_IRQ | CPSR_FLAG_FIRQ);
	// set mode
	tmp |= (is_sys ? MODE_SYS : MODE_USR);
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
	swap(&processes[current].context, context, &processes[next].context);
	current = next;
}


void swap(cpu_context* curr, u32* hw_context, cpu_context* next) {

	printf("Swap status words:\r\n");
	printf_cpsr(curr->cpsr);

	printf_cpsr(next->cpsr);

	// save r0-12, lr as pc
	memcpy(curr, hw_context, sizeof(curr->registers) + sizeof(u32));
	
	//save cpsr and move in new one, hw copied/banked it to spsr
	asm(
		"mrs %0, spsr\n"
		"msr spsr, %1\n"
		: "=&r" (curr->cpsr)
		: "r" (next->cpsr)
		:
	);

	//save sp & lr, move in new ones
	u32 a,b;

	/* This version uses more register shenans, but is 4 instructoins longer
	asm volatile(
		"MRS %[tmp1],CPSR\n"
		"BIC %[tmp2],%[tmp1],#0x1F\n"
		"ORR %[tmp2],%[tmp2],#0b11111\n"
		"MSR cpsr, %[tmp2]\n"
		"mov %[curr_sp], sp\n"
		"mov sp, %[next_sp]\n"
		"mov %[curr_lr], lr\n"
		"mov lr, %[next_lr]\n"
		"MSR cpsr, %[tmp1]\n"
		: [curr_sp] "=&r" (curr->sp), [curr_lr] "=&r" (curr->lr), [tmp1] "=&r" (a), [tmp2] "=&r" (b) // "=&r" happens also kills some warnings we'd like enabled for scratch vars, but oh well
		: [next_sp] "r" (next->sp), [next_lr] "r" (next->lr)
		: "sp", "lr" // gcc is not happy about this, but otherwise it has ideas of using these
	);*/

	asm volatile(
		"MRS %[tmp1],CPSR\n"
		"BIC %[tmp2],%[tmp1],#0x1F\n"
		"ORR %[tmp2],%[tmp2],#0b11111\n"
		"MSR cpsr, %[tmp2]\n"
		"str sp, %[curr_sp]\n"
		"ldr sp, %[next_sp]\n"
		"str lr, %[curr_lr]\n"
		"ldr lr, %[next_lr]\n"
		"MSR cpsr, %[tmp1]\n"
		: [curr_sp] "=&o" (curr->sp), [curr_lr] "=&o" (curr->lr), [tmp1] "=&r" (a), [tmp2] "=&r" (b) // "=&r" happens also kills some warnings we'd like enabled for scratch vars, but oh well
		: [next_sp] "o" (next->sp), [next_lr] "o" (next->lr)
		: "sp", "lr" // gcc is not happy about this, but otherwise it has ideas of using these
	);

	// move new context to stack, to be restored upon return
	memcpy(hw_context, next->registers, sizeof(next->registers) + sizeof(u32));
	// to create new threads, just zero this out.
	// or be friendly and put a call to exit into the lr
}