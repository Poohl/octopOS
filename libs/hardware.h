
#ifndef HARDWARE_H

#include "interfaces.h"

/**
 * This is the interface all boards need to implement beyond the call to main
 */

int debug_put_char(char c);
int debug_get_char();
sequence_io_status debug_write(uint len, const byte* data);
sequence_io_status debug_read(uint len, byte* buff);

/**
 * This is the api the general OS needs to implement
 */

#define EXCEPTION_UNKNOWN_INST 1
#define EXCEPTION_ILLEGAL_DATA 2
#define EXCEPTION_ILLEGAL_CODE 3
#define EXCEPTION_ILLEGAL_INST 4

void exception_handler(int type, void* instruction, void* data);
void syscall_handler(int syscall, void* source);


#endif