#ifndef VECTORS_H
#define VECTORS_H

#include "default.h"

void switch_vector_table();

void set_svc_handler(void (*hand)());

void set_undef_instr_vector(void (*hand)());

typedef struct {
	u32 remap;
	const u32 abort_status;
	const u32 abort_address_status;
	u32 master_prio;
} ui;

#endif