
#ifndef PRINTF_H
#define PRINF_H

#include "default.h"
#include "drivers/unbuffered_byte_stream.h"

void printf_init(void* _outstream, sequence_io_status(*_write)(void*, uint, const byte*));

sequence_io_status printf(char* format, ...);

#endif
