
extern "C" {
#include "process_mgmt.h"
#include "default.h"
#include "libs/hardware.h"
}
#include "drivers/Streams.hpp"
#include "drivers/Callback.hpp"
#include "drivers/Timer.hpp"

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

class UnblockIOThreadCallback : public Callback<sequence_io_status> {
	public:
		uint tid;
		UnblockIOThreadCallback() : tid(0) {};
		UnblockIOThreadCallback(uint t) : tid(t) {};
		void call(sequence_io_status out) {
			unblock_fancy(tid, NULL, out.err ? (u32*) &out.err : (u32*) &out.io, 1);
		}
};

static UnblockIOThreadCallback unblock_io_storage[16];
static char char_storage[16];

uint wait_for_io_callback(u32* hw_context) {
	uint tid = block_current(hw_context);
	unblock_io_storage[tid] = UnblockIOThreadCallback(tid);
	return tid;
}

extern AsyncOutStream* debug_out_stream;
extern AsyncInStream* debug_in_stream;


void impl_debug_getchar(u32* hw_context) {
	uint slot = wait_for_io_callback(hw_context);
	debug_in_stream->read(&char_storage[slot], 1, &unblock_io_storage[slot]);
}

void impl_debug_putchar(u32* hw_context, char c) {
	uint slot = wait_for_io_callback(hw_context);
	char_storage[slot] = c;
	debug_out_stream->write(&char_storage[slot], 1, &unblock_io_storage[slot]);
}

class UnblockSleepThreadCallback : public Callback<> {
	public:
		uint tid;
		UnblockSleepThreadCallback() : tid(0) {};
		UnblockSleepThreadCallback(uint t) : tid(t) {};
		void call() {
			unblock(tid);
		};
};

static MultiAlarm* sleepAlarm;
static UnblockSleepThreadCallback unblock_sleep_storage[16];

void impl_sleep(u32 delay) {
	uint tid = block_current(get_stacked_context());
	unblock_sleep_storage[tid] = UnblockSleepThreadCallback(tid);
	sleepAlarm->start(delay, &unblock_sleep_storage[tid]);
}

extern "C" { 
void init_syscalls(MultiAlarm* _sleepAlarm) {
	sleepAlarm = _sleepAlarm;
	void_void_func_ptr init[] = {
		syscall_context(&exit),
		syscall_nocontext(&new_thread),
		syscall_nocontext(&new_thread_raw),
		syscall_context(&impl_debug_putchar),
		syscall_context(&impl_debug_getchar),
		syscall_context(&impl_sleep)
	};
	memcpy(syscall_table, init, sizeof(init));
}

void unhandled_syscall() {
	exception_handler(EXCEPTION_UNKNOWN_SYSCALL, NULL, get_stacked_context());
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
	asm volatile("bx lr");
}

__attribute__ ((noinline))
int sys_debug_get_char() {
	do_syscall(4);
	asm volatile("bx lr");
}

__attribute__ ((noinline))
void sys_sleep(u32 ticks) {
	do_syscall(5);
}

}

#pragma GCC diagnostic pop
