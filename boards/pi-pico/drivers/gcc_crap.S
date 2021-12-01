/* 
 * Before complaining about any of our code
 * Explain to me how macros inside inline assembly work
 * And then explain to me why they work this way
 * and then give me a good reason why the syntax is such shit
 * and then look at this and be happy there's no "\r\n #"#X crap here
 */

#include "board.h"

.global init_stacks
init_stacks:
	mrs r0, control
	mov r1, #CONTROL_PROC_STACK
	ldr r2, =#INIT_PROCESS_STACK
	ORR r0, r1
	msr control, r0
	mov sp, r2
	bic r0, r1
	msr control, r0
	bx lr
