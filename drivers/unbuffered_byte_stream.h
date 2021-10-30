
#ifndef UNBUFFERED_BYTE_STREAM_H
#define UNBUFFERED_BYTE_STREAM_H

#include "default.h"
#include "interfaces.h"

sequence_io_status unbuff_out_write(if_drv_byte_outstream* child, uint len, const byte* data);

sequence_io_status unbuff_out_read(if_drv_byte_instream* child, uint len, byte* buff);

#endif
