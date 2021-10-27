
.section .init, "ax"
.global _start
_start:
	b c_entry
//will never get here, no stack means no return
deadlock:
	add r1, r2, r3
	b deadlock
