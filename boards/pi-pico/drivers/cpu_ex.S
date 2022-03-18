

.section "text"

isr_return_exit:
	pop {pc}
.global switch_on_isr_return
switch_on_isr_return:
	ldr r3, =switch_context
	ldr r2, [r3, #0]
	cmp r2, #0
	beq isr_return_exit
	mov r2, #0
	str r2, [r3, #0]
	pop {r2}
	ldr r0, [r3, #4]
	ldr r1, [r3, #8]
	str r2, [r0, #32]
	ldr r2, [r1, #32]

// these high registers are a joke
// there are literally 2 1/2 instructions able to access them
// and two of those are mov...
swap_high_registers: //(cpu_context.registers from, const cpu_context.registers to, u32 return_magic), return address on stack
	stmia r0!, {r4-r7}
	mov r4, r8
	mov r5, r9
	mov r6, r10
	mov r7, r11
	stmia r0!, {r4-r7}
	mov r3, #16
	add r3, r1, r3
	ldmia r3!, {r4-r7}
	mov r8, r4
	mov r9, r5
	mov r10, r6
	mov r11, r7
	ldmia r1!, {r4-r7}
	bx r2

	