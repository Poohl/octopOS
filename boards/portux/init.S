
.section .init, "ax"
.global _start
_start:
	ldr     SP,=0x00270000  // and we have a stack... randomly sitting in the SRAM
	bl      c_entry

.Lend:
//	wfi                     // find peace
.word 0xe320f003
	b       .Lend           // nirvana-loop

.global RANDO_CODE
RANDO_CODE:
	b 0x26ff00				// i wonder what will happen