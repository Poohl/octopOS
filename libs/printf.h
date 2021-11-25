
#ifndef PRINTF_H
#define PRINTF_H

#include "hardware.h"

__attribute__((format(printf, 1, 2))) 
sequence_io_status printf(const char* format, ...);

void multiprint(char c, int n);

#endif
