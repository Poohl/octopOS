.section vectors

// the bounds of what needs to appear at 0x0
.global vector_table
.global vector_table_end

// varibales to store the destination addresses in.
.global reset_vector
.global undef_instr_vecor
.global software_interrupt_vector
.global code_load_abort_vector
.global data_load_abort_vector
.global interrupt_vector
.global fast_interrupt_vector

// helper to initiallize the sp in a banked mode
.global vectors_init
// the sp to set it to
.global sp_set
// helper to return from exception
.global return_from_vector


vector_table:
ldr PC, reset_vector
ldr PC, undef_instr_vecor
ldr PC, software_interrupt_vector
ldr PC, code_load_abort_vector
ldr PC, data_load_abort_vector
ldr PC, interrupt_vector
.word 0x0
ldr PC, fast_interrupt_vector
.word 0x0

reset_vector:
.word _start
undef_instr_vecor:
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
.vector_table_end:

.return_from_vector:
ldm sp, {pc}