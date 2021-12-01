#include "default.h"
#include "board.h"
#include "libs/hardware.h"

typedef void_void_func_ptr isr; 

static void dontCare() {
}

static void _exception_hand() {
	u32 _lr;
	asm ("mov %0, lr" : "=r" (_lr) : :);
	u32* exception_sp;
	if (_lr != ISR_RETURN_THREAD_PROC) {
		asm ("mrs %0, msp" : "=r" (exception_sp) : :);
	} else {
		asm ("mrs %0, psp" : "=r" (exception_sp) : :);
	}
	u32 exception_pc = exception_sp[8];
	exception_sp[8] += 2;
	exception_handler(0, exception_pc, *((default_instruction*) (exception_pc)));
}

static void _svcall_hand() {
	syscall_handler(0, NULL);
}

isr vectors[] __attribute__((section(".vectors"))) = {
	[0 ... 16+32] = &dontCare,
	[0] = (isr) 0x20001000,
	[1] = &_start,
	// [2] = &_nmi_hand, // nonmaskable interrupt
	[3] = &_exception_hand,
	[11] = &_svcall_hand, // hard syscall
	[14] = &_svcall_hand, // soft syscall
	[15] = &timer_handler,
	// [16 ... 16+32] = &_interrupt_hand
};
