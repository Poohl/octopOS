

#include "default.h"

#define AP_NONE_NONE 0b00
#define AP_RW_NONE 0b01
#define AP_RW_R 0b10
#define AP_RW_RW 0b11

typedef struct {
    u32 desc[4096];
} first_level_pagetable __attribute__ ((aligned (16384)));

int get_address_space();

void release_address_space(uint id);

void init_first_level_pagetable(uint address_space_id);

int set_ap(uint address_space_id, void* address, uint size, void* dest, uint ap);

void swap_address_space(uint src, uint dest);

void init_mmu_mgmt();

void init_mmu();
