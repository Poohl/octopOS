
#include "default.h"
#include "memory-map.h"
#include "vectors.h"
#include "libs/hardware.h"
#include "aic.h"
#include "libs/printf.h"

extern void (*reset_vector)();
extern void (*undef_instr_vector)();
extern void (*software_interrupt_vector)();
extern void (*code_load_abort_vector)();
extern void (*data_load_abort_vector)();
extern void (*interrupt_vector)();
extern void (*fast_interrupt_vector)();
extern void UNDEF_ex();
extern void D_ABORT_ex();
extern void C_ABORT_ex();
extern void SWI();
extern void UNDEF_ISR();

volatile aic* _aic;

__attribute__((interrupt ("IRQ")))
static void undef_interrupt_hand() {
	printf("Interrupt\r\n");
	exception_handler(EXCEPTION_UNEXPECTED_ISR, NULL, ((aic*) AIC)->status);
	_aic->signal_end = 1;
	_aic->clear = 1 << _aic->status;
}

void init_vector_handling() {
	_aic = (aic*) AIC;
	reset_vector = SWI;
	code_load_abort_vector = C_ABORT_ex;
	data_load_abort_vector = D_ABORT_ex;
	undef_instr_vector = UNDEF_ex;
	software_interrupt_vector = SWI;
	volatile ui* user_friendly_interface = (ui*) (USER_INTERFACE);
	user_friendly_interface->remap = 1;
	_aic->spurious_target = &undef_interrupt_hand;
	
	/*_aic->isr_target[3] = &undef_interrupt_hand;
	_aic->isr_mode[3] = 0;*/

	for (int i = 0; i < 32; i++) {
		_aic->isr_target[i] = &undef_interrupt_hand;
		_aic->isr_mode[i] = 0;
	}
}

void set_interrupt_handler(u8 num, void_void_func_ptr handler, u8 prio, u8 mode) {
	if (num < 32 && prio <= 7 && mode <= 3) {
		_aic->isr_target[num] = handler;
		_aic->isr_mode[num] = (mode << 5) | prio;
	}
}