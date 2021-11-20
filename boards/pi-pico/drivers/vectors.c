#include "default.h"
#include "board.h"
#include "libs/hardware.h"

typedef void_void_func_ptr isr; 

__attribute__((isr))
static void dontCare() {
}

__attribute__((isr))
static void _exception_hand() {
	exception_handler(0, NULL, NULL);
}

__attribute__((isr))
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
	// [15] = &_systick_hand,
	// [16 ... 16+32] = &_interrupt_hand
};
