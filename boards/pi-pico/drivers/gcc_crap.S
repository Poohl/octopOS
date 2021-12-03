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
	ldr r0, =#INIT_PROCESS_STACK
	msr psp, r0
	bx lr
