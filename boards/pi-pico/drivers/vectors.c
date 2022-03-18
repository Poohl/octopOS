#include "default.h"
#include "board.h"
#include "libs/hardware.h"
#include "vectors.h"
#include "cpu.h"

typedef void_void_func_ptr isr; 

extern void swap_thread_context();

/**
 * Note: tnis function is really not compatible with anything
 * and only works as intended when
 * - The calling function doesn't access registers 4-11 after the call
 * - gcc uses tailchaing for the return
 * - the stars align to form a vaid assmembly insruction
 */
/*void switch_context() {
	asm volatile("")
}*/

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

#define LOAD_STACK_ISR_WRAPPER(func) \
	__attribute__((naked)) \
	static void _##func () { \
		asm( \
			"mov r1, lr \n" \
			"add r1, #(" STRINGIFY((0 - ISR_RETURN_THREAD_PROC)) ") \n" \
			"beq _"#func"_proc_stack \n" \
			"mrs r0, MSP \n" \
			"b _" #func "_switch \n" \
		"_"#func"_proc_stack: \n" \
			"mrs r0, PSP \n" \
		"_"#func"_switch: \n" \
			"mov r1, lr \n" \
			"add r1, #(" STRINGIFY((0 - ISR_RETURN_HANDLER)) ") \n" \
			"beq "#func"\n" \
			"push {lr} \n" \
			"bl "#func"\n" \
			"b switch_on_isr_return" \
		); \
	}

#define ISR_WRAPPER(func) \
	__attribute__((naked)) \
	static void _##func () { \
		asm( \
			"mov r1, lr \n" \
			"add r1, #(" STRINGIFY((0 - ISR_RETURN_HANDLER)) ") \n" \
			"beq "#func"\n" \
			"push {lr} \n" \
			"bl "#func"\n" \
			"b switch_on_isr_return" \
		); \
	}

void _exception_hand(hw_cpu_context* context) {
	exception_handler(0, context->pc, *((default_instruction*) (context->pc)));
}

LOAD_STACK_ISR_WRAPPER(_exception_hand)

void _svcall_hand(hw_cpu_context* context) {
	u16 syscall = *(((u16*) context->pc) - 1) & 0xFF;
	syscall_handler(syscall, context);
}

LOAD_STACK_ISR_WRAPPER(_svcall_hand)

static void _unexpected_isr_hand() {
	u32 num;
	asm("mrs %0, ipsr" : "=r" (num) : : );
	exception_handler(EXCEPTION_UNEXPECTED_ISR, NULL, num);
}

//LOAD_STACK_ISR_WRAPPER(systick_hand)

isr vectors[] __attribute__((section(".vectors"))) = {
	[0 ... 16] = &dontCare,
	[0] = (isr) 0x20001000,
	[1] = &_start,
	// [2] = &_nmi_hand, // nonmaskable interrupt
	[3] = &__exception_hand,
	// [4 ... 10] = reserved
	[11] = &__svcall_hand, // hard syscall
	// [12,13] = reserved
	[14] = &__svcall_hand, // soft syscall
	[15] = &systick_hand,
	[16 ... 16+32] = &_unexpected_isr_hand,
	[16 + 20] = &uart0_hand
};

volatile nvic* _nvic = (nvic*) NVIC;

void init_stacks() {
	u32 value = INIT_PROCESS_STACK;
	asm volatile("msr psp, %0" :  : "r" (value) : );
}

void init_vectors() {
	_nvic->enable = (1 << 20);
	asm volatile("cpsie i" : : : );
}

u32* get_stacked_context(void) {
	return NULL;
}

void test() {
	_nvic->set = 1 << 20;
}