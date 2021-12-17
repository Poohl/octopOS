
#ifndef PROCESS_MGMT_H
#define PROCESS_MGMT_H

#include "default.h"

#include "hardware.h"

void init_process_mgmt();

int exit();

int new_thread(thread_args* args);

void thread_swap_callback(u32* context);

#endif
