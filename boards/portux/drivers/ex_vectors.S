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

// helpers for c code
.global init_stacks
// helper to return from exception
.global return_from_vector

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

return_from_vector:
ldm sp, {pc}

.global UNDEF_ex
UNDEF_ex:
push {r0-r12, lr}
mov r0, #EXCEPTION_ILLEGAL_INST
mov r1, lr
add r1, r1, #-4 
ldr r2, [r1]
bl  exception_handler
// mov lr, r0
ldm sp!, {r0-r12, pc}^

.global D_ABORT_ex
D_ABORT_ex:
push {r0-r12, lr}
mov r0, #EXCEPTION_ILLEGAL_DATA
mov r1, lr
add r1, r1, #-8
ldr r2, [r1]
bl  exception_handler
// mov lr, r0
ldm sp!, {r0-r12, pc}^

.global C_ABORT_ex
C_ABORT_ex:
push {r0-r12, lr}
mov r0, #EXCEPTION_ILLEGAL_CODE
mov r1, lr
add r1, r1, #-4 
//ldr r2, [r1]
bl  exception_handler
// mov lr, r0
ldm sp!, {r0-r12, pc}^

.global SWI
SWI:
push {r0-r12, lr}
mov r1, lr
add r1, r1, #-4 
ldr r2, [r1]
bl  syscall_handler
// mov lr, r0
ldm sp!, {r0-r12, pc}^
