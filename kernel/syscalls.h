
#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "process_mgmt.h"
#include "default.h"
#include "libs/hardware.h"

#define SYSCALLS 6

void init_syscalls(void_void_func_ptr syscall_init[SYSCALLS]);

void unhandled_syscall(u32 a, u32 b, u32 c, u32 d);

__attribute__ ((noinline))
void sys_exit();

__attribute__ ((noinline))
void sys_new_thread( char* name,  init_thread_state_args* args);

__attribute__ ((noinline))
void sys_restore_thread( char* name,  cpu_context* context);

__attribute__ ((noinline))
int sys_debug_put_char( char c);

__attribute__ ((noinline))
int sys_debug_get_char();

__attribute__ ((noinline))
void sys_sleep( u64 nsec);

#endif
