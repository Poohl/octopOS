#include "default.h"
#include "board.h"
#include "libs/hardware.h"
#include "drivers/uart.h"

typedef void_void_func_ptr isr; 

typedef struct {
	u32 enable;
	u32 _reserved1[0x7b / 4 + 1];
	u32 disable;
	u32 _reserved2[0x7b / 4 + 1];
	u32 set;
	u32 _reserved3[0x7b / 4 + 1];
	u32 clear;
	u32 _reserved4[0xFC / 4 + 1];
	u32 prio[8];
	u32 _reserved5[0x1C / 4 + 1];
} nvic;

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

static void _unexpected_isr_hand() {
	u32 num;
	asm("mrs %0, ipsr" : "=r" (num) : : );
	exception_handler(EXCEPTION_UNEXPECTED_ISR, NULL, num);
}

isr vectors[] __attribute__((section(".vectors"))) = {
	[0 ... 16] = &dontCare,
	[0] = (isr) 0x20001000,
	[1] = &_start,
	// [2] = &_nmi_hand, // nonmaskable interrupt
	[3] = &_exception_hand,
	// [4 ... 10] = reserved
	[11] = &_svcall_hand, // hard syscall
	// [12,13] = reserved
	[14] = &_svcall_hand, // soft syscall
	[15] = &timer_handler,
	[16 ... 16+32] = &_unexpected_isr_hand,
	[16 + 20] = &uart0_hand
};

static volatile nvic* _nvic = (nvic*) NVIC;

void init_stacks() {
	u32 value = INIT_PROCESS_STACK;
	asm volatile("msr psp, %0" :  : "r" (value) : );
}

void init_vectors() {
	_nvic->enable = (1 << 20);
	asm volatile("cpsie i" : : : );
}