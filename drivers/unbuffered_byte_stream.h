
#ifndef UNBUFFERED_BYTE_STREAM_H
#define UNBUFFERED_BYTE_STREAM_H

#include "default.h"

typedef struct {
	void* byte_device;
	int (*put_byte)(void*, byte);
} unbuffered_outstream_driver;

typedef struct {
	void* byte_device;
	int (*get_byte)(void*);
} unbuffered_instream_driver;

typedef struct {
	uint io;
	int err;
} sequence_io_status;

sequence_io_status unbuff_out_write(unbuffered_outstream_driver* self, uint len, const byte* data);

sequence_io_status unbuff_out_read(unbuffered_instream_driver* self, uint len, byte* buff);

#endif
