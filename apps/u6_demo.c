

#include "kernel/syscalls.h"
#include "kernel/process_mgmt.h"
#include "libs/printf.h"
#include "libs/delay.h"

__attribute__ ((section (".usertext.rodata"))) 
char intro[] = 
            "Please Select the kind of MMU fault you wish to trigger:\r\n"
            "1: Read Nullpointer\r\n"
            "2: Read Kernel data\r\n"
            "3: Execute Kernel code\r\n"
            "4: Write own code\r\n"
            "5: Stackoverflow\r\n"
            "6: read undefined memeory\r\n"
            "7: Access Sram that was mapped to undef (abort)\r\n"
            "8: Access Undef (abort) that was mapped to sram\r\n";

__attribute__ ((section (".usertext"))) int brick_it() {
    while (1) {
    for (int i = 0; i < 287; ++i)
        sys_debug_put_char(intro[i]);
    volatile u32 test;
    switch (sys_debug_get_char()) {
        case '1':
            test = *((u32*) 0x0);
            break;
        case '2':
            test = *((u32*) init_process_mgmt);
            break;
        case '3':
            init_process_mgmt();
            break;
        case '4':
            *((u32*) brick_it) = 4;
            break;
        case '5':
            asm volatile(
                "mov r0,  #1\n"
                "mov r1,  #2\n"
                "mov r2,  #3\n"
                "mov r3,  #4\n"
                "mov r4,  #5\n"
                "mov r5,  #6\n"
                "mov r6,  #7\n"
                "mov r7,  #8\n"
                "mov r8,  #9\n"
                "mov r9,  #10\n"
                "mov r10, #11\n"
                "mov r11, #12\n"
                "mov r12, #13\n"
                "push {r0-r12}\n"
                "push {r0-r12}\n"
                "push {r0-r12}\n"
                "push {r0-r12}\n"
                "add pc, pc, #-20"
                : : : "memory"
            );
            break;
        case '6':
            test = *((u32*) (UNDEF_MEMORY1 + MEGABYTE + 200));
            break;
        case '7':
            test = *((u32*) (EXTERNAL_SRAM + 17*MEGABYTE));
            break;
        case '8':
            test = *((u32*) (UNDEF_MEMORY1));
            break;
    }
    }
}