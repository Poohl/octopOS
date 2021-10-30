
#ifndef PRINTF_H
#define PRINF_H

#include "default.h"
#include "drivers/dbgu.h"

__attribute__((format(printf, 1, 2))) 
sequence_io_status printf(char* format, ...);

#endif
