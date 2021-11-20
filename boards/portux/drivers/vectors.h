#ifndef VECTORS_H
#define VECTORS_H

#include "default.h"

extern void init_stacks();

void init_vector_handling();

typedef struct {
	u32 remap;
	const u32 abort_status;
	const u32 abort_address_status;
	u32 master_prio;
} ui;

#endif