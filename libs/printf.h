
#ifndef PRINTF_H
#define PRINTF_H

#include "default.h"
#include "interfaces.h"

extern sequence_io_status (*write)(uint len, const byte *data);

extern sequence_io_status (*read)(uint len, byte *data);

__attribute__((format(printf, 1, 2))) 
sequence_io_status printf(char* format, ...);

void multiprint(char c, int n);

#endif
