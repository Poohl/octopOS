

#include "default.h"
#include "libs/hardware.h"
#include "memory-map.h"
#include "libs/printf.h"

typedef u32 default_instruction;

static default_instruction payload[64+1];

extern default_instruction in_tr_ret, in_tr_nop;

void injection_trainer();


static void help() {
	printf(
		"# This is the help page for the arbitrary code injection trainer #\r\n"
		"1. General operation\r\n"
		"  You are presented with a prompt for hexadecimal encoded data. Said data is decoded\r\n"
		"  and copied into memory, then executed as function when you enter $.\r\n"
		"  You have space for 64 instructions of standard size."
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
		"  and paste the resulting bytes in here.\r\n"
		"\r\n"
		"4. Fancy Features\r\n"
		"  To ease debugging, a pretty standard printf is available at %p and the game loop is at %p.\r\n"
		"\r\n"
		"5. Examples\r\n"
		"  If You just wanna test it working, here's a few examples and what they should do:\r\n"
		"5.1 Portux\r\n"
		"   5.1.1 -> 0x23232323 is an undefind instruction\r\n"
		"   5.1.2 -> 0xef000001 causes a syscall\r\n"
		"   5.1.3 -> 0xe51f0000\r\n"
		"         -> 0xe5900000\r\n"
		"         -> 0x0fffff00 causes a data abort\r\n"
		"   5.1.4 -> 0xe51ff004\r\n"
		"         -> 0x0fffff00 causes a code abort\r\n"
		"   5.1.5 -> 0xe320f003 wfi aka kill the cpu\r\n"
		"",
		&printf, &injection_trainer
	);
}


default_instruction toInt(char* hexStr) {
	default_instruction ret = 0;
	int asciiOffset;

	for (uint i = 0; i < sizeof(default_instruction) * 2; i++) {
		if (hexStr[i] < 58) 
			asciiOffset = 48;
		else 
			asciiOffset = 87;
		ret += (hexStr[i] - asciiOffset) * (1 << 4*(7-i));
	}
	return ret;
}

int inject_code() {
	char buffer[sizeof(default_instruction)*2];
	int instrcnt = 0;
	int i = 0;

	/* read input */
	do {
		printf("\n\rinject @ %p -> 0x", &payload[instrcnt]);
		for (i = 0; i < sizeof(default_instruction)*2; i++) {
			buffer[i] = debug_get_char();

			/* backspace function */
			if (buffer[i] == 0x7f) {
				printf("\n\rinject @ %p -> 0x", &payload[instrcnt]);
				for (int j = 0; j < i-1; j++) debug_put_char(buffer[j]);
				i -= 2;
				continue;
			};

			debug_put_char(buffer[i]);
			if (buffer[i] == '$') break;
		} 

		if (buffer[i] != '$')
			payload[instrcnt] = toInt(buffer);
		instrcnt++;
	} while (buffer[i] != '$');
	printf("\n\rhere goes nothing...\n\r");

	return instrcnt;
}


void injection_trainer() {
	int score = 0;
	int p = 0;
	while (1) {
		printf("Welcome to the Injection trainer 1.0, $ for help, Current score %x\r\n", score);
		// clear the injection buffer
		for (uint i = 0; i < ARR_LEN(payload) - sizeof(default_instruction); ++i)
			payload[i] = in_tr_nop;
		payload[ARR_LEN(payload) - sizeof(default_instruction)] = in_tr_ret;
		p = inject_code();
		if (p <= 1)
			help();

		((void_void_func_ptr) &payload)();
		score += p >> 2;
		if (p > 0)
			score += p >> 2;
	}
}
//775F0074
//74005F7774005F7774005F77