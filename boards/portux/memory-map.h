
#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H


#define MODE_USR 0b10000
#define MODE_FIQ 0b10001
#define MODE_IRQ 0b10010
#define MODE_SVC 0b10011 /* reset */
#define MODE_ABT 0b10111
#define MODE_UND 0b11011
#define MODE_SYS 0b11111

#define INTERNAL_SRAM  0x200000
#define INIT_STACK     0x270000

#define INIT_FIQ_STACK 0x280100
#define INIT_IRQ_STACK 0x280200
#define INIT_SVC_STACK 0x280500
#define INIT_ABT_STACK 0x280300
#define INIT_UND_STACK 0x280400
#define INIT_SYS_STACK INIT_STACK

#define USER_INTERFACE 0xFFFFFF00

#define DBGU 0xFFFFF200
#define DBGU_END 0xFFFFF400

#define AIC 0xFFFFF000

#define SYSTEM_TIMER 0xFFFFFD00

#define enable_interrupts() asm volatile ( \
		"MRS r0,CPSR\n" \
		"BIC r0,r0,#0b11000000\n" \
		"MSR cpsr, r0\n" \
		: : : "r0")

#endif
