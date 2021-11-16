
#include "default.h"
#include "memory-map.h"
#include "vectors.h"

extern void (*reset_vector)();
extern void (*undef_instr_vector)();
extern void (*software_interrupt_vector)();
extern void (*code_load_abort_vector)();
extern void (*data_load_abort_vector)();
extern void (*interrupt_vector)();
extern void (*fast_interrupt_vector)();

void set_svc_handler(void (*hand)()) {
	reset_vector = hand;
}

void set_undef_instr_vector(void (*hand)()) {
	undef_instr_vector = hand;
}

void switch_vector_table() {
	volatile ui* user_friendly_interface = (ui*) (USER_INTERFACE);
	user_friendly_interface->remap = 1;
}


