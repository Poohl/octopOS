
#define SRAM_BASE 0x10000000
#define VECTORS_BASE 0x10000100

#define INIT_MAIN_STACK 0x20001000
#define INIT_PROCESS_STACK 0x20002000

#define SYS_CTL_VTOR 0xE000ED08

#ifndef __ASSEMBLER__

void _start(void);

typedef u16 default_instruction;

#endif