
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

