#ifndef PORTUX_BOARD_H
#ifdef BOARD_H
#error "You already included" BOARD_H "'s board define, cannot include multiple"
#endif
#define PORTUX_BOARD_H
#define BOARD_H "armv4 portux"

#define INTERNAL_SRAM    0x200000

#define INIT_STACK       0x270000
#define INIT_SYS_STACK INIT_STACK
#define INIT_FIQ_STACK   0x280000
#define INIT_IRQ_STACK   0x281000
#define INIT_ABT_STACK   0x282000
#define INIT_UND_STACK   0x283000
#define INIT_SVC_STACK   0x284000

#define AIC            0xFFFFF000

#define DBGU           0xFFFFF200

#define DBGU_END       0xFFFFF400

#define SYSTEM_TIMER   0xFFFFFD00
#define USER_INTERFACE 0xFFFFFF00



#define MODE_USR 0b10000
#define MODE_FIQ 0b10001
#define MODE_IRQ 0b10010
#define MODE_SVC 0b10011 /* reset */
#define MODE_ABT 0b10111
#define MODE_UND 0b11011
#define MODE_SYS 0b11111

#ifdef __ASSEMBLER__

#define enable_interrupts \
	MRS r0,CPSR \
	BIC r0,r0,#0b11000000 \
	MSR cpsr, r0

#else /* actual C code */

typedef u32 default_instruction;

#define enable_interrupts() \
	u32 _go_to_heck_gcc; \
	asm volatile ( \
		"MRS %0,CPSR\n" \
		"BIC %0,%0,#0b11000000\n" \
		"MSR cpsr, %0\n" \
		: "=&r" (_go_to_heck_gcc) : : "memory")

typedef struct {
	u32 registers[13];
	u32 pc;
	u32 sp;
	u32 lr;
	u32 cpsr;
} cpu_context;

#endif

#endif