
#ifndef PRINTF_H
#define PRINF_H

#include "default.h"
#include "drivers/unbuffered_byte_stream.h"

void printf_init(const if_drv_outsteam* _out_handle);

sequence_io_status printf(char* format, ...);

#endif
