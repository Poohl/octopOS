
#include "default.h"

#include "hardware.h"
#include "libs/printf.h"

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

static const char* exception_fmt[] = {
	"Undefined exception at %p, additional info: %p\r\n",
	"Illegal data access from %p by %p\r\n",
	"Illegal code access to %p, extra data %p\r\n",
	"Illegal instruction at %p, instruction %p\r\n",
	"During %p: Unhandeled interrupt %p\r\n",
	"From %p: Undefined syscall %p\r\n",
	"From %p: Illegal syscall%p\r\n",
	"Unknown exception at %p, additional info: %p\r\n"
};

#define exception_fmt_len (sizeof(exception_fmt)/sizeof(exception_fmt[0]))

__attribute__((weak)) void exception_handler(int type, void* instruction, void* data) {
	printf(exception_fmt[type], instruction, data);
	while (1);
}

__attribute__((weak)) void syscall_handler(int syscall, void* source) {
	printf("Syscall %x triggered at %p\r\n", syscall, source);
}

__attribute__((weak)) void timer_handler() {
	debug_write(3, "!\r\n");
}

__attribute__((weak)) void
__attribute__ ((section (".usertext")))
*memset(void *s, int c, size_t n) {
	for (size_t i = 0; i < n; ++i)
		((byte*) s)[i] = c;
	return s;
}

__attribute__((weak))
__attribute__ ((section (".usertext")))
void *memcpy(void *dest, const void *src, size_t n) {
	for (size_t i = 0; i < n; ++i)
		((byte*) dest)[i] = ((byte*) src)[i];
	return dest;
}