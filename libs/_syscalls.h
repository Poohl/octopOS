
#ifndef LIB_SYSCALLS_H
#define LIB_SYSCALLS_H


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