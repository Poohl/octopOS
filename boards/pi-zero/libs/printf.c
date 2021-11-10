#include <stdarg.h>
#include "printf.h"
#include "../interfaces.h"

static void add_to_sequence_io_status(sequence_io_status* dest, const sequence_io_status* src) {
	dest->io += src->io;
	dest->err |= src->err;
}

sequence_io_status printf(char* format, ...) {
	va_list args;
	va_start(args, format);
	char *cursor = format, c, *str, buff[10];
	const char* lookuptable = "0123456789abcdef";
	int len, pos;
	sequence_io_status out = {
		.io = 0,
		.err = 0
	};
	sequence_io_status hw_out;
	/*
	 * How this works:
	 * format points at remainder of the format string,
	 * cursor runs ahead and points at whatever is going to be formatted next.
	 * 
	 * the loop then explosts the fact that a format sting is a sequence of %?
	 * with constant strings in between. Thus it loops as follows:
	 * 
	 * while format left:
	 *   write constant string
	 *   if format left:
	 *     pop arg
	 *     format arg
	 *     write arg
	 * end:
	 *   free(args)
	 */
	while (*cursor) {
		// run to the next '%'
		for (cursor = format; *cursor && *cursor != '%'; ++cursor);
		hw_out = dbgu_write(cursor - format, format);
		add_to_sequence_io_status(&out, &hw_out);
		format = cursor;
		if (out.err || !*cursor)
			goto end;

		++cursor; // cursor is at a '%' here, the foramt char is next.
		switch (*cursor) {
			case 0: // format string ends on a '%', we're friendly and print it.
				hw_out = dbgu_write(1, cursor - 1);
				break;
			case 'c':
				c = (char) va_arg(args, int);
				hw_out = dbgu_write(1, &c);
				break;
			case 's':
				str = va_arg(args, char*);
				for (len = 0; str[len]; ++len);
				hw_out = dbgu_write(len, str);
				break;
			default:
				hw_out = dbgu_write(5, "%ERR:");
				add_to_sequence_io_status(&out, &hw_out);
				if (out.err) 
					goto end;
				/* fall through */
			case 'x':
				/* fall through */
			case 'p':
				// write the number into the buffer, back to front.
				pos = 9;
				for (u32 num = va_arg(args, u32); num; num >>= 4, --pos)
					buff[pos] = lookuptable[num & 0xF];
				buff[pos] = 'x';
				buff[--pos] = '0';
				hw_out = dbgu_write(10-pos, &buff[pos]);
				break;
		}
		add_to_sequence_io_status(&out, &hw_out);
		if (out.err)
			goto end;
		++cursor;
		format = cursor;
	}
end:
	va_end(args);
	return out;
}
