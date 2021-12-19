
#include "board.h"

.section .init, "ax"
.global _start
_start:
	// Start in system mode
	MRS r0,CPSR
	BIC r0,r0,#0x1F
	ORR r0,r0,#MODE_SYS
	MSR cpsr, r0
	ldr     SP,=INIT_STACK  // and we have a stack... randomly sitting in the SRAM
	bl      c_entry

.Lend:
//	wfi                     // find peace
.word 0xe320f003
	b       .Lend           // nirvana-loop

.section .text, "ax"
.global system_interrupt_hand
system_interrupt_hand:
	sub lr, lr, #4
	push {r0-r12, lr}
	bl	dbgu_interupt_callback
	bl	timer_interrupt_callback
	cmp r0, #0
	movne r0, sp
	blne thread_swap_callback
	// I swear to god gcc, if I find a solution to this: PC + imm = AIC + 0x130 I'll come over an murder you
	ldr r0, =#AIC+0x130
	str r0, [r0]
	ldm	sp!, {r0-r12, pc}^
