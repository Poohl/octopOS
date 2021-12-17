
#include "default.h"
#include "board.h"
#include "cpu.h"
#include "libs/misc.h"
#include "kernel/process_mgmt.h"

void tight_powersave() {
	asm volatile(".word 0xe320f003");
}

void init_context(cpu_context* context, thread_args* args) {
	memset(context, sizeof(*context), 0);
	context->pc = args->start;
	context->lr = args->exit;
	if (args->stack)
		context->sp = args->stack;
	else
		context->sp = &context->registers;
	u32 tmp;
	asm ("mrs %0, cpsr" : "=r" (tmp) : :);
	tmp = (tmp & ~0x1F) | (args->is_sys ? MODE_SYS : MODE_USR);
	context->cpsr = tmp;
}

void swap(cpu_context* curr, u32* hw_context, cpu_context* next) {
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