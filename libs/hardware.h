#include "interfaces.h"

/**
 * This is the interface all boards need to implement beyond the call to main
 */

#ifndef HARDWARE_H

int debug_put_char(char c);
int debug_get_char();
sequence_io_status debug_write(uint len, const byte* data);
sequence_io_status debug_read(uint len, byte* buff);

#endif