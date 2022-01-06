
#include "default.h"
#include "board.h"
#include "cpu.h"
#include "libs/printf.h"

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

void tight_powersave() {
	asm volatile(".word 0xe320f003");	// MINE BITCOINS HERE
}

void cpu_context_init(cpu_context* dest, const init_thread_state_args* src) {
	memset(dest, sizeof(*dest), 0);
	memcpy(&dest->registers, &src->args, sizeof(src->args));
	memset(&dest->registers[ARR_LEN(src->args)], 0, sizeof(dest->registers) - sizeof(src->args));
	dest->pc = src->start;
	dest->lr = src->exit;
	dest->sp = src->stack + (src->stack_size >> 2);
	u32 tmp;
	asm ("mrs %0, cpsr" : "=r" (tmp) : :);
	// clear mode & interrupt mask
	tmp &= ~(CPSR_FLAG_MODE_MASK | CPSR_FLAG_IRQ | CPSR_FLAG_FIRQ);
	// set mode
	tmp |= (src->is_sys ? MODE_SYS : MODE_USR);
	dest->cpsr = tmp;
}

bool cpu_context_validate(cpu_context* dest, bool may_be_sys) {
	// either you are allowed anything or you are user with interrupts
	return may_be_sys ||
		(
			(dest->cpsr & CPSR_FLAG_MODE_MASK == MODE_USR)
		&& !(dest->cpsr & (CPSR_FLAG_IRQ | CPSR_FLAG_FIRQ))
		)
	;
}

void swap(cpu_context* curr, u32* hw_context, cpu_context* next) {

	//printf("Swap status words:\r\n");
	//printf_cpsr(curr->cpsr);
	//printf_cpsr(next->cpsr);

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

void set_return_values(cpu_context* dest, u32* values, uint count) {
	switch (count) {
		default:
		case 2:
			dest->registers[1] = values[1];
		case 1:
			dest->registers[0] = values[0];
		case 0:
			break;
	}
}