
#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#define INTERNAL_SRAM 0x200000

#define PROCESS_STAKC

#define DBGU 0xFFFFF200
#define DBGU_END 0xFFFFF400

#ifndef __ASSEMBLER__

typedef u32 default_instruction;

#endif

#endif
