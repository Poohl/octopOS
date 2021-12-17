
#ifndef HARDWARE_H
#define HARDWARE_H

#include "board.h"

/**
 * This is the interface all boards need to implement beyond the call to kernel
 */

// WARNING: THIS FILE IS INCLUDED FROM ASSEMBLER AND HAS TO BE GUARDED FOR THIS

#ifndef __ASSEMBLER__

/**
 * Struct to hold the result of multiple sequential IO operations, as we don't have errno
 * io: number of sucessfull io operations
 * err: error on the (io+1)th operation or 0 if all requests were sucessfull
 */
typedef struct {
	uint io;
	int err;
} sequence_io_status;

// debug IO
int debug_put_char(char c);
int debug_get_char();
sequence_io_status debug_write(uint len, const byte* data);
sequence_io_status debug_read(uint len, byte* buff);

// timer
int set_timer_interval(uint period);

// process mgmt
typedef struct {
	void* exit;
	void* stack;
	void* start;
	u32 args[8];
	bool is_sys;
} thread_args;
/**
 * This function performs the cpu-context switch from one thread to 
 * the next.
 * - first the actual context is stored in curr.
 * - then the context from next is swapped in for the actual context.
 * Most notably this function does _not_ itself apply the next context,
 * it just sets up the application for the return from the
 * system_inter_hand.
 * 
 * args:
 * @curr: where to store the actual context
 * @hw_context: whatever the system_inter_hand passed to the thread_swap_callback.
 *              Usually some pointer to pushed registers.
 * @next: the next context to apply
 */
void swap(cpu_context* curr, u32* hw_context, cpu_context* next);
void init_context(cpu_context* context, thread_args* args);
void tight_powersave();

#endif

/**
 * This is the api the hardware needs to call at appropiate times
 */

// Excpetion numbers.
// It is understood that most hardware is not able to differentiate all of them.
#define EXCEPTION_UNKNOWN 0
#define EXCEPTION_ILLEGAL_DATA 1
#define EXCEPTION_ILLEGAL_CODE 2
#define EXCEPTION_ILLEGAL_INST 3
#define EXCEPTION_UNEXPECTED_ISR 4

// actions to perform after the return of a functoin that returns one
// this is done because it enables lazy initiallisatoin of additional parameters
#define ACTION_NONE 0
#define ACTION_THREAD_SWAP_CALLBACK (1 << 0)

#ifndef __ASSEMBLER__

typedef int exception_enum;

typedef uint action_enum;

action_enum exception_handler(int type, void* instruction, void* data);
action_enum syscall_handler(int syscall, void* source);
action_enum timer_handler();
void thread_swap_callback(u32* context);

#endif

#endif