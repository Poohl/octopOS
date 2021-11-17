
#include "default.h"
#include "memory-map.h"
#include "vectors.h"
#include "libs/hardware.h"

extern void (*reset_vector)();
extern void (*undef_instr_vector)();
extern void (*software_interrupt_vector)();
extern void (*code_load_abort_vector)();
extern void (*data_load_abort_vector)();
extern void (*interrupt_vector)();
extern void (*fast_interrupt_vector)();


__attribute__((interrupt ("UNDEF")))
static void illegal_instr_hand() {
	exception_handler(EXCEPTION_ILLEGAL_INST, NULL, NULL);
}

__attribute__((interrupt ("ABORT")))
static void code_load_abort_hand() {
	exception_handler(EXCEPTION_ILLEGAL_CODE, NULL, NULL);
}

__attribute__((interrupt ("ABORT")))
static void data_load_abort_hand() {
	exception_handler(EXCEPTION_ILLEGAL_DATA, NULL, NULL);
}

__attribute__((interrupt ("SWI")))
static void software_interrupt_hand() {
	syscall_handler(0, NULL);
}

void init_vector_handling() {
	reset_vector = &software_interrupt_hand;
	code_load_abort_vector = code_load_abort_hand;
	data_load_abort_vector = data_load_abort_hand;
	undef_instr_vector = illegal_instr_hand;
	volatile ui* user_friendly_interface = (ui*) (USER_INTERFACE);
	user_friendly_interface->remap = 1;
}


