
#ifndef DBGU_H
#define DBGU_H

#include "default.h"
#include "interfaces.h"

int dbgu_put_byte(byte c);

int dbgu_get_byte();

sequence_io_status dbgu_write(uint len, const byte* data);

sequence_io_status dbgu_read(uint len, byte* buff);

#endif