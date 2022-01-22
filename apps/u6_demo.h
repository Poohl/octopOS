

#include "kernel/syscalls.h"
#include "kernel/process_mgmt.h"
#include "libs/printf.h"
#include "libs/delay.h"


__attribute__ ((section (".usertext"))) int brick_it();
