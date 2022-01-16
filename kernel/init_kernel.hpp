
#ifndef INIT_KERNEL_HPP
#define INIT_KERNEL_HPP

#include "drivers/Timer.hpp"

void init_process_mgmt(PeriodicTimer* _timer);

void init_syscalls(MultiAlarm* _alarm);

#endif
