
#ifndef HARDWARE_H
#define HARDWARE_H

/**
 * This is the interface all boards need to implement beyond the call to main
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


int debug_put_char(char c);
int debug_get_char();
sequence_io_status debug_write(uint len, const byte* data);
sequence_io_status debug_read(uint len, byte* buff);

#endif

/**
 * This is the api the general OS needs to implement
 */

#define EXCEPTION_UNKNOWN 0
#define EXCEPTION_ILLEGAL_DATA 1
#define EXCEPTION_ILLEGAL_CODE 2
#define EXCEPTION_ILLEGAL_INST 3
#define EXCEPTION_UNEXPECTED_ISR 4

#ifndef __ASSEMBLER__

void exception_handler(int type, void* instruction, void* data);
void syscall_handler(int syscall, void* source);

#endif

#endif