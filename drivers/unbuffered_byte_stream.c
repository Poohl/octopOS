
#include "default.h"

#include "unbuffered_byte_stream.h"

sequence_io_status unbuff_out_write(if_drv_byte_outstream* child, uint len, const byte* data) {
	sequence_io_status out = {};
	for (out.io = 0; out.io < len &&
			!(out.err = (*child->put_byte)(child->byte_device, data[out.io]));
		++out.io);
	if (out.err)
		out.io -= 1;
	return out;
}

sequence_io_status unbuff_out_read(if_drv_byte_instream* child, uint len, byte* buff) {
	sequence_io_status out = {};
	for (out.io = 0; out.io < len &&
			0 <= (out.err = (*child->get_byte)(child->byte_device));
		++out.io) {
			buff[out.io] = (byte) out.err;
		}
	if (out.err < 0)
		out.io -= 1;
	else
		out.err = 0;
	return out;
}