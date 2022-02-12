

#include "kernel/syscalls.h"
#include "kernel/process_mgmt.h"
#include "libs/printf.h"
#include "libs/delay.h"

__attribute__ ((section (".usertext"))) int child(char c, char n, volatile u32* global_counter_p) {
    for (u32 local_counter = 0; local_counter < 3; ++local_counter, ++*global_counter_p) {
        sys_debug_put_char(c);
        sys_debug_put_char(n);
        sys_debug_put_char(' ');
        sys_debug_put_char(*global_counter_p + '0');
        sys_debug_put_char(' ');
        sys_debug_put_char(local_counter + '0');
        sys_debug_put_char('\r');
        sys_debug_put_char('\n');
        sys_sleep(0, 1000);
    }
}

__attribute__ ((section (".usertext"))) int lead(char c) {
    u32 global_counter = 0;
    init_thread_state_args child_args = default_init_thread_state_args;
    child_args.new_address_space = false;
    child_args.args[0] = c;
    child_args.args[1] = '2';
    child_args.args[2] = &global_counter;
    child_args.start = (void_void_func_ptr) child;
    char name[7] = {
        'c', 'h', 'i', 'l', ' ', c, '2'
    };
    sys_new_thread(name, &child_args);
    name[6] += 1;
    child_args.args[1] += 1;
    sys_new_thread(name, &child_args);
    child(c, '1', &global_counter);
}


__attribute__ ((section (".usertext"))) int u7_demo() {
    while (1) {
        init_thread_state_args next_thread = default_init_thread_state_args;
        next_thread.start = (void_void_func_ptr) lead;
        next_thread.args[0] = sys_debug_get_char();
        char name[7] = {
            'l', 'e', 'a', 'd', ' ', (char) next_thread.args[0]
        };
        sys_new_thread(name, &next_thread);
    }
}
