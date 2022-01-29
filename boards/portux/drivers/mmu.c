

#include "mmu.h"
#include "board.h"

static first_level_pagetable l1table;

void init_first_level_pagetable() {
    // all initiallization applies to domain 0
    // 1:1 mapping for all memories with priv-only access
    for (uint i = 0; i < 4096; ++i) {
        l1table.desc[i] = (i << 20) | (AP_RW_NONE << 10) | 0b10;
    }
    // No access to Undefined memory even for priv
    for (uint i = (UNDEF_MEMORY1 >> 20); i < 1519; ++i)
        l1table.desc[i] = (AP_NONE_NONE << 10) | 0b10;
    for (uint i = (UNDEF_MEMORY2 >> 20); i < 249; ++i)
        l1table.desc[i] = (AP_NONE_NONE << 10) | 0b10;
    // make userland acessible by users
    uint userland_desc = EXTERNAL_SRAM >> 20;
    l1table.desc[userland_desc] = (userland_desc << 20) | (AP_RW_R << 10) | 0b10;
    for (uint i = 0; i < 63; ++i) {
        l1table.desc[userland_desc + 1 + i] |= (AP_RW_RW << 10);
    }
    // modify the mapping to switch 1MB of sram with 1MB of undef memory
    l1table.desc[(EXTERNAL_SRAM + 17*MEGABYTE) >> 20] = (UNDEF_MEMORY1) | (AP_RW_RW << 10) | 0b10;
    l1table.desc[(UNDEF_MEMORY1) >> 20] = (EXTERNAL_SRAM + 17*MEGABYTE) | (AP_RW_RW << 10) | 0b10;
}

void init_mmu() {
    asm volatile( // program to be domain 0 client
        "mov r0, #0x1\n"
        "mcr p15, 0, r0, c3, c0, 0"
        : : : "memory");
    asm volatile( // set page-base-table
        "mcr p15, 0, %[table], c2, c0, 0"
        : 
        : [table] "r" (l1table.desc)
        : "memory"
    );
    asm volatile( // enable mmu
        "mrc p15, 0, r0, c1, c0, 0\n"
        "orr r0, r0, #0x1\n"
        "mcr p15, 0, r0, c1, c0, 0\n"
        : : : "r0", "memory"
    );
}