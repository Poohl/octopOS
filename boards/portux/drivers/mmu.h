

#include "default.h"

#define AP_NONE_NONE 0b00
#define AP_RW_NONE 0b01
#define AP_RW_R 0b10
#define AP_RW_RW 0b11

typedef struct {
    u32 desc[4096];
} first_level_pagetable __attribute__ ((aligned (16384)));

void init_first_level_pagetable();

void init_mmu();
