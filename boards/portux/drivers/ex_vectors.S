#include "board.h"
#include "libs/hardware.h"

.section .vectors, "ax"

// the bounds of what needs to appear at 0x0
.global vector_table
.global vector_table_end

// varibales to store the destination addresses in.
.global reset_vector
.global undef_instr_vector
.global software_interrupt_vector
.global code_load_abort_vector
.global data_load_abort_vector
.global interrupt_vector
.global fast_interrupt_vector

// full-decending stack pointer to currently saved context
.global context_pointer_stack

// helpers for c code
.global init_stacks

vector_table:
ldr PC, reset_vector
ldr PC, undef_instr_vector
ldr PC, software_interrupt_vector
ldr PC, code_load_abort_vector
ldr PC, data_load_abort_vector
.word 0x6675636b // buffer to fill 4 bytes
//ldr PC, [AIC + offsetof(aic, current_target)] // gcc's legendary pipeline can't handle this
ldr PC, [PC, #AIC+0x100-24-8]                   // so we assemble it a bit more manually
//                ^ offset of current_target
//                      ^ offset of this instruction from 0x0 after remap
//                         ^ offset of PC from current instruction
// effectively: PC + AIC + offsetof(aic, current_target) - PC
.word 0x0
ldr PC, fast_interrupt_vector

reset_vector:
.word _start
undef_instr_vector:
.word _start
software_interrupt_vector:
.word _start
code_load_abort_vector:
.word _start
data_load_abort_vector:
.word _start
interrupt_vector:
.word _start
fast_interrupt_vector:
.word _start
vector_table_end:

.section .text, "ax"
init_stacks:
MRS r0,CPSR
BIC r0,r0,#0x1F
ORR r0,r0, #MODE_FIQ
//ORR r0,r0, 0x13
MSR cpsr, r0
ldr sp, =INIT_FIQ_STACK

MRS r0,CPSR
BIC r0,r0,#0x1F
ORR r0,r0,#MODE_IRQ
MSR cpsr, r0
ldr sp, =INIT_IRQ_STACK

MRS r0,CPSR
BIC r0,r0,#0x1F
ORR r0,r0,#MODE_ABT
MSR cpsr, r0
ldr sp, =INIT_ABT_STACK

MRS r0,CPSR
BIC r0,r0,#0x1F
ORR r0,r0,#MODE_UND
MSR cpsr, r0
ldr sp, =INIT_UND_STACK

MRS r0,CPSR
BIC r0,r0,#0x1F
ORR r0,r0,#MODE_SVC
MSR cpsr, r0
ldr sp, =INIT_SVC_STACK

MRS r0,CPSR
BIC r0,r0,#0x1F
ORR r0,r0,#MODE_SYS
MSR cpsr, r0

bx lr


context_pointer_stack_base:
.word 0x0
.word 0x0
.word 0x0
.word 0x0
.word 0x0
.word 0x0
.word 0x0
.word 0x0
.word 0x0
.word 0x0
.word 0x0
.word 0x0
.word 0x0
.word 0x0
.word 0x0
.word 0x0
context_pointer_stack_top:
context_pointer_stack:
.word context_pointer_stack_top

.macro PUSH_CONTEXT TMP
push {r0-r12, lr}
ldr \TMP, context_pointer_stack
str sp, [\TMP, #-4]!
str \TMP, context_pointer_stack
.endm

.macro DISCARD_CONTEXT ZERO, TMP
and \ZERO, \ZERO, #0
ldr \TMP, context_pointer_stack
str \ZERO, [\TMP], #4
str \TMP, context_pointer_stack
.endm

.global UNDEF_ex
UNDEF_ex:
PUSH_CONTEXT r2
mov r0, #EXCEPTION_ILLEGAL_INST
add r1, lr, #-4 
bl  exception_handler
DISCARD_CONTEXT r0, lr
ldm sp!, {r0-r12, pc}^

.global D_ABORT_ex
D_ABORT_ex:
PUSH_CONTEXT r2
mov r0, #EXCEPTION_ILLEGAL_DATA
add r1, lr, #-8
bl  exception_handler
DISCARD_CONTEXT r0, lr
ldm sp!, {r0-r12, pc}^

.global C_ABORT_ex
C_ABORT_ex:
PUSH_CONTEXT r2
mov r0, #EXCEPTION_ILLEGAL_CODE
add r1, lr, #-4
bl  exception_handler
DISCARD_CONTEXT r0, lr
ldm sp!, {r0-r12, pc}^

.global system_interrupt_hand
system_interrupt_hand:
	sub lr, lr, #4
	PUSH_CONTEXT r0
	bl	dbgu_interupt_callback
	bl	timer_interrupt_callback
	/*cmp r0, #0
	movne r0, sp
	blne thread_swap_callback*/
	// I swear to god gcc, if I find a solution to this: PC + imm = AIC + 0x130 I'll come over an murder you
	ldr r0, =#AIC+0x130 
	str r0, [r0]
	DISCARD_CONTEXT r0, r1
	ldm	sp!, {r0-r12, pc}^

#define SYSCALLS 4

.global SWI
/* C AAPCS (almost, consider this a function-prolouge)
 * if the syscall has the lowest bit set, it's not thumb, it means it'll get the context in r0
 */
SWI: 
push {lr}

// get the syscall number from the instruction
ldr ip, [lr, #-4]
and ip, ip, #0x00FFFFFF
// load the routine
cmp ip, #(SYSCALLS + 1)
ldr lr, =syscall_table
ldrhs ip, =unhandled_syscall
ldrlo ip, [lr, ip, LSL#2] // syscall_table + idx * 4
tst ip, #1
pushne {r0-r12}
ldr lr, context_pointer_stack
strne sp, [lr, #-4]!
addeq lr, lr, #-4
str lr, context_pointer_stack
ldrne lr, =SWI_context_ret
ldreq lr, =SWI_quick_ret
movne r0, sp
bic pc, ip, #1

SWI_quick_ret:
DISCARD_CONTEXT r3, r2
and r2, r2, #0
ldm sp!, {pc}^

SWI_context_ret:
DISCARD_CONTEXT r2, r3
ldm sp!, {r0-r12, pc}^
