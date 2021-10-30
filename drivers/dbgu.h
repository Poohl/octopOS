
#ifndef DBGU_H
#define DBGU_H

#include "default.h"
#include "interfaces.h"

int dbgu_init(if_hw_mem_dbgu* child);

int dbgu_put_byte(if_hw_mem_dbgu* child, byte c);

int dbgu_get_byte(if_hw_mem_dbgu* child);

void print(char* str);
#endif