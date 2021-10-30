
#include "memory-map.h"
#include "drivers/dbgu.h"
#include "drivers/unbuffered_byte_stream.h"
#include "printf.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")


void c_entry(void) {

	if_hw_mem_dbgu* dbgu = (if_hw_mem_dbgu*) DBGU;
	dbgu_init(dbgu);
	dbgu_put_byte(dbgu, '#');

	if_drv_byte_outstream dbgu_out = {
		.byte_device = dbgu,
		.put_byte = &dbgu_put_byte
	};
	unbuff_out_write(&dbgu_out, 14, "\nPlain write\n");

	if_drv_outsteam dbgu_out_stream = {
		.outstream = &dbgu_out,
		.write = &unbuff_out_write
	};

	printf_init(&dbgu_out_stream);

	printf("This is a simple test\n");

	printf("This is a more complicated test %c %x %s\n", '1', 0x23, "456");

}

#pragma GCC pop_options