
#include "hardware.h"
#include "interfaces.h"

__attribute__((weak)) sequence_io_status debug_write(uint len, const byte* data) {
	sequence_io_status out = {};
	for (out.io = 0; out.io < len &&
			!(out.err = debug_put_char(data[out.io]));
		++out.io);
	if (out.err)
		out.io -= 1;
	return out;
}

__attribute__((weak)) sequence_io_status debug_read(uint len, byte* buff) {
	sequence_io_status out = {};
	for (out.io = 0; out.io < len &&
			0 <= (out.err = debug_get_char());
		++out.io) {
			buff[out.io] = (byte) out.err;
		}
	if (out.err < 0)
		out.io -= 1;
	else
		out.err = 0;
	return out;
}