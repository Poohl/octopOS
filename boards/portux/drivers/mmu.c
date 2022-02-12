

#include "mmu.h"
#include "board.h"

static first_level_pagetable address_spaces[8];

typedef struct {
    uint buff[8];
    uint available;
    uint next;
} addsp_queue;

addsp_queue addsp_q;

void init_addsp_queue(addsp_queue* q) {
	for (int i = 0; i < 8; ++i)
		q->buff[i] = i;
	q->available = 7;
	q->next = 0;
}

int get_addsp_slot(addsp_queue* q){
	if (q->available > 0) {
		int ret = q->buff[q->next];
		q->available --;
		q->next = q->next + 1 & 0x7;
		return ret;
	} else {
		return -1;
	}
}

void release_addsp_slot(addsp_queue* q, int slt) {
	q->buff[q->next + q->available & 0x7] = slt;
	q->available++;
}

void init_mmu_mgmt() {
    init_addsp_queue(&addsp_q);
}

int get_address_space() {
    int id = get_addsp_slot(&addsp_q);
    if (id < 0)
        return id;
    init_first_level_pagetable(id);
    return id;
}

void release_address_space(uint id) {
    release_addsp_slot(&addsp_q, id);
}

void init_first_level_pagetable(uint address_space_id) {
    // all initiallization applies to domain 0
    // 1:1 mapping for all memories with priv-only access
    for (uint i = 0; i < 4096; ++i) {
        address_spaces[address_space_id].desc[i] = (i << 20) | (AP_RW_NONE << 10) | 0b10;
    }
    // No access to Undefined memory even for priv
    for (uint i = (UNDEF_MEMORY1 >> 20); i < 1519; ++i)
        address_spaces[address_space_id].desc[i] = (AP_NONE_NONE << 10) | 0b10;
    for (uint i = (UNDEF_MEMORY2 >> 20); i < 249; ++i)
        address_spaces[address_space_id].desc[i] = (AP_NONE_NONE << 10) | 0b10;
    // make userland acessible by users
    uint userland_desc = EXTERNAL_SRAM >> 20;
    address_spaces[address_space_id].desc[userland_desc] = (userland_desc << 20) | (AP_RW_R << 10) | 0b10;
    /*for (uint i = 0; i < 63; ++i) {
        address_spaces[address_space_id].desc[userland_desc + 1 + i] |= (AP_RW_RW << 10);
    }*/
    // modify the mapping to switch 1MB of sram with 1MB of undef memory
    //address_spaces[address_space_id].desc[(EXTERNAL_SRAM + 17*MEGABYTE) >> 20] = (UNDEF_MEMORY1) | (AP_RW_RW << 10) | 0b10;
    //address_spaces[address_space_id].desc[(UNDEF_MEMORY1) >> 20] = (EXTERNAL_SRAM + 17*MEGABYTE) | (AP_RW_RW << 10) | 0b10;
}

int set_ap(uint address_space_id, void* address, uint size, void* dest, uint ap) {
    if (size != MEGABYTE)
        return -1;
    address_spaces[address_space_id].desc[(u32) address >> 20] = (((u32) dest) & (~0 << 20)) | (ap << 10) | 0b10;
}

void init_mmu() {
    asm volatile( // program to be domain 0 client
        "mov r0, #0x1\n"
        "mcr p15, 0, r0, c3, c0, 0"
        : : : "memory");
    asm volatile( // set page-base-table
        "mcr p15, 0, %[table], c2, c0, 0"
        : 
        : [table] "r" (address_spaces[7].desc)
        : "memory"
    );
    asm volatile( // enable mmu
        "mrc p15, 0, r0, c1, c0, 0\n"
        "orr r0, r0, #0x1\n"
        "mcr p15, 0, r0, c1, c0, 0\n"
        : : : "r0", "memory"
    );
}

void swap_address_space(uint src, uint dest) {
    if (src == dest)
        return;
    asm volatile(
        "MCR p15, 0, %0, c8, c7, 0\n" // invalidate caches
        "mcr p15, 0, %0, c7, c7, 0\n" // invalidate tlb
        :
        : "r" (0)
        : "memory"
    );
    
    asm volatile( // set page-base-table
        "mcr p15, 0, %[table], c2, c0, 0\n"
        : 
        : [table] "r" (address_spaces[dest].desc)
        : "memory"
    );
}

