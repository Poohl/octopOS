
#include "default.h"
#include "libs/hardware.h"
#include "process_mgmt.h"

#define SYSCALLS 6

#define do_syscall(NUM) \
	asm volatile( \
		"swi %0" \
		: : "i" (NUM) : \
	)

#define syscall_context(X) \
	(void_void_func_ptr) (((uint) (X)) | 1)

void_void_func_ptr syscall_table[SYSCALLS];

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic ignored "-Wreturn-type"

int impl_get_char(u32* hw_context);

void init_syscalls() {
	// fun fact: sizeof(syscall_init) is 4, sizeof(syscall_table) is 4*SYSCALLS
	// Why? because gcc devs live in houses with opaque windows.
	void_void_func_ptr syscall_init[] = {
		syscall_context(&exit),
		&new_thread,
		&new_thread_raw,
		&debug_put_char,
		syscall_context(&impl_get_char),
		syscall_context(&sleep)
	};
	memcpy(syscall_table, syscall_init, sizeof(syscall_table));
}

void unhandled_syscall(u32 a, u32 b, u32 c, u32 d) {
	exception_handler(EXCEPTION_UNKNOWN_SYSCALL, NULL, NULL);
}

static int getc_block_thread_id;

void impl_get_char_callback(byte c) {
	unblock(getc_block_thread_id, c);
}

int impl_get_char(u32* hw_context) {
	debug_get_char_async(impl_get_char_callback);
	block_current(hw_context);
}

__attribute__ ((noinline))
void sys_exit() {
	do_syscall(0);
}

__attribute__ ((noinline))
void sys_new_thread(char* name,  void* args) {
	do_syscall(1);
}

__attribute__ ((noinline))
void sys_restore_thread(char* name,  void* context) {
	do_syscall(2);
}

__attribute__ ((noinline))
int sys_debug_put_char(char c) {
	do_syscall(3);
}

__attribute__ ((noinline))
int sys_debug_get_char() {
	do_syscall(4);
}

__attribute__ ((noinline))
void sys_sleep(u32 _ignore, u32 delay) {
	do_syscall(5);
}

#pragma GCC diagnostic pop
