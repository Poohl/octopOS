

#include "default.h"
#include "libs/hardware.h"
#include "memory-map.h"
#include "libs/printf.h"

typedef u32 default_instruction;

static default_instruction injection_buffer[64+1];

extern default_instruction in_tr_ret, in_tr_nop;

void injection_trainer();

// taken from https://gitlab.com/wireshark/wireshark/-/blob/master/ui/text_import.c
#define INVALID_VALUE (-1)
#define WHITESPACE_VALUE (-2)
#define TERMINATE_VALUE (-3)
#define INVALID_INIT [0 ... 255] = INVALID_VALUE
#define WHITESPACE_INIT \
	[' '] = WHITESPACE_VALUE, \
	['\t'] = WHITESPACE_VALUE, \
	['\n'] = WHITESPACE_VALUE, \
	['\v'] = WHITESPACE_VALUE, \
	['\f'] = WHITESPACE_VALUE, \
	['\r'] = WHITESPACE_VALUE

sbyte hex_decode_table[256] = {
		INVALID_INIT,
		WHITESPACE_INIT,
		['\n'] = TERMINATE_VALUE,
		['\r'] = TERMINATE_VALUE,
		[':'] = WHITESPACE_VALUE,
		['0'] = 0,1,2,3,4,5,6,7,8,9,
		['A'] = 10,11,12,13,14,15,
		['a'] = 10,11,12,13,14,15
};

static int parse_plain_data() {
	byte* dest = (byte*) injection_buffer;
	ubyte src;
	sbyte val;
	int status = 1;
	ubyte c_val = 0;
	uint c_chars = 0;
	int bytes = 0;
	while (dest < (byte*) (injection_buffer + ARR_LEN(injection_buffer))) {
		val = hex_decode_table[src = debug_get_char()];
		debug_put_char(src);
		switch (val) {
		  case INVALID_VALUE:
			status = -1;
		  case TERMINATE_VALUE:
			goto remainder;
		  case WHITESPACE_VALUE:
			break;
		  default:
			c_val = c_val << 4 | val;
			++c_chars;
			/* another full unit */
			if (c_chars == 2) {
				++bytes;
				c_chars = 0;
				*dest = c_val & 0xFF;
				++dest;
			}
		}
	}
remainder:
	if (c_chars > 0)
		*dest = val & 0xFF;
	debug_put_char('\r');
	debug_put_char('\n');
	return status * bytes;
}

static void help() {
	printf(
		"# This is the help page for the arbitrary code injection trainer #\r\n"
		"1. General operation\r\n"
		"  You are presented with a prompt for hexadecimal encoded data. Said data is decoded\r\n"
		"  and copied into memory, then executed as function when you press enter.\r\n"
		"  Be aware of byteorder!, endianess is handled for you. Data is copied byte by byte.\r\n"
		"  You have space for 64 instructions of standard size. If you enter partial instructions\r\n"
		"  the remaining bytes will be filled with corresondings bytes from 'nop'.\r\n"
		"  The execution of your payload works roughly as follows:\r\n"
		"    your_code:\r\n"
		"      <whatever you entered>\r\n"
		"      nop (to fill up to 64 instructions)\r\n"
		"      bx lr\r\n"
		"    aci_trainer:\r\n"
		"      bl your_code\r\n"
		"      bl aci_trainer\r\n"
		"\r\n"
		"2. Recovery\r\n"
		"  Exceptions are handeled and the faulty instructions skipped. Other than that none,\r\n"
		"  if you mess up the cpu state big time you'll have to reset it.\r\n"
		"\r\n"
		"3. Getting started\r\n"
		"  Easiest is probably to just write the assembly you would like to inject, assemble it\r\n"
		"  and paste the resulting bytes in here (probably in reverse because of byteorder)\r\n"
		"\r\n"
		"4. Fancy Features\r\n"
		"  To ease debugging, a pretty standard printf is available at %p and the game loop is at %p.\r\n"
		"\r\n"
		"5. Examples\r\n"
		"  If You just wanna test it working, here's a few examples and what they should do:\r\n"
		"5.1 Portux\r\n"
		"    23232323 is an undefind instruction\r\n"
		"    XXXXXXXX causes a syscall\r\n"
		"",
		&printf, &injection_trainer
	);
}

void injection_trainer() {
	int score = 0;
	int p = 0;
	while (1) {
		printf("Welcome to the Injection trainer 1.0, enter for help, Current score %x\r\n", score);
		// clear the injection buffer
		for (uint i = 0; i < ARR_LEN(injection_buffer) - sizeof(default_instruction); ++i)
			injection_buffer[i] = in_tr_nop;
		injection_buffer[ARR_LEN(injection_buffer) - sizeof(default_instruction)] = in_tr_ret;
		p = parse_plain_data();
		if (p <= 0)
			help();

		((void_void_func_ptr) &injection_buffer)();
		score += p >> 2;
		if (p > 0)
			score += p >> 2;
	}
}
//775F0074
//74005F7774005F7774005F77