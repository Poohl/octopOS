
extern "C" {
#include "process_mgmt.h"
#include "default.h"
#include "libs/hardware.h"
}
#define SYSCALLS 6

#define do_syscall(NUM) \
	asm volatile( \
		"swi %0" \
	: : "i" (NUM) : )

#define syscall_context(X) \
	(void_void_func_ptr) (((uint) (X)) | 1)

#define syscall_nocontext(X) \
	(void_void_func_ptr) (X)

void_void_func_ptr syscall_table[SYSCALLS];

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-fpermissive"
#pragma GCC diagnostic ignored "-Wreturn-type"

extern "C" { 
void init_syscalls() {
	void_void_func_ptr init[] = {
		syscall_context(&exit),
		syscall_nocontext(&new_thread),
		syscall_nocontext(&new_thread_raw),
		syscall_nocontext(&debug_put_char),
		syscall_context(&debug_get_char),
	};
	memcpy(syscall_table, init, sizeof(init));
}

void unhandled_syscall(u32 a, u32 b, u32 c, u32 d) {
	exception_handler(EXCEPTION_UNKNOWN_SYSCALL, NULL, NULL);
}

__attribute__ ((noinline))
void sys_exit() {
	do_syscall(0);
}

__attribute__ ((noinline))
void sys_new_thread(char* name,  init_thread_state_args* args) {
	do_syscall(1);
}

__attribute__ ((noinline))
void sys_restore_thread(char* name,  cpu_context* context) {
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
void sys_sleep(u64 nsec) {
	do_syscall(5);
}

}

#pragma GCC diagnostic pop
