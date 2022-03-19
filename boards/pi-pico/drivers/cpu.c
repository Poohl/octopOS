
#include "cpu.h"

void cpu_context_init(cpu_context* dest, const struct _init_thread_state_args* src) {
	memset(dest, sizeof(*dest), 0);
	memcpy(&dest->low_registers, &src->args, sizeof(src->args));
	memset(&dest->registers[ARR_LEN(src->args)], 0, sizeof(dest->registers) - sizeof(src->args));
	dest->return_const = ISR_RETURN_THREAD_PROC;
	dest->pc = (u32) src->start & ~1;
	dest->lr = (u32) src->exit;
	dest->sp = (u32) (src->stack + (src->stack_size >> 2)) - sizeof(hw_cpu_context);
	dest->xpsr = (1 << 24);
	dest->control = (1 << 1) | (src->is_sys ? 0 : 1);
	memcpy((u32*) (dest->sp), dest, sizeof(hw_cpu_context));
}

bool cpu_context_validate(cpu_context* dest, bool may_be_sys) {
	return (dest->xpsr & 0x0fffffff) == (1 << 24)
			&& (dest->control & 1 || may_be_sys);
}

void tight_powersave() {
	asm volatile("wfi");
}

struct {
bool pending;
u32* from;
const u32* to;
} switch_context;

void swap(cpu_context* curr, u32* _ignore, cpu_context* next) {
	asm ("mrs %0, psp" : "=r" (curr->sp) : :);
	asm ("msr psp, %0" : : "r" (next->sp) :);
	switch_context.from = curr->registers;
	switch_context.to = next->registers;
	switch_context.pending = true;
}

void call_for_hw_context(void_void_func_ptr func, hw_cpu_context* context) {
	asm volatile(
		"ldmia %[context], {r0-r3} \n"
		"add %[context], #-16 \n"
		"blx %[func] \n"
		"stmia %[context], {r0-r3} \n"
	: 
	: [context] "r" (context), [func] "r" (func)
	: "r0", "r1", "r2", "r3", "lr"
	);
}

void set_return_values(cpu_context* dest, u32* values, uint count) {
	switch (count) {
		default:
		case 2:
			dest->low_registers[1] = values[1];
		case 1:
			dest->low_registers[0] = values[0];
		case 0:
			break;
	}
}
