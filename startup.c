
#include "memory-map.h"
#include "drivers/dbgu.h"
#include "drivers/unbuffered_byte_stream.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")


void c_entry(void) {

	dbgu_controller_driver dbgu = {
		.controller = (volatile struct dbgu_mem_layout *) DBGU
	};
	dbgu_init(&dbgu);

	unbuffered_outstream_driver dbgu_out = {
		.byte_device = &dbgu,
		.put_byte = &dbgu_put_byte
	};
	unbuff_out_write(&dbgu_out, 4, "test");

	print("Hello world! :)\n");
}

#pragma GCC pop_options