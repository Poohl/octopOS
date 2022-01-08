
#include "kernel/syscalls.h"
#include "kernel/process_mgmt.h"
#include "libs/printf.h"
#include "libs/delay.h"

void write_active(char c) {
	for (int i = 0; i < c; ++i) {
		sys_debug_put_char(c);
		worstdelayever(100);
	}
}

void write_passive(char c) {
	for (int i = 0; i < c; ++i) {
		sys_debug_put_char(c);
		sys_sleep(0, 10000);
	}
}

void reader() {
	while (1) {
		int c = sys_debug_get_char();
		init_thread_state_args next_thread = default_init_thread_state_args;
		next_thread.args[0] = c;
		if ('a' <= c  && c <='z') {
			next_thread.start = &write_passive;
		} else if ('A' <= c && c <= 'Z') {
			next_thread.start = &write_active;
		} else {
			printf("Unknwon char %c\r\n", c);
		}
		sys_new_thread("", &next_thread);
	}
}