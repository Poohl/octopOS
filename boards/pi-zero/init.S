.section .init, "ax"
.globl _start
_start:
    mov sp,#0x8000
    /* mov r0,pc */
    bl c_entry
.Lend: b .Lend

.globl PUT32
PUT32:
    str r1,[r0]
    bx lr

.globl GET32
GET32:
    ldr r0,[r0]
    bx lr

.globl dummy
dummy:
    bx lr

