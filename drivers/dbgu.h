
#ifndef DBGU_H
#define DBGU_H

#include "default.h"

struct dbgu_mem_layout;
typedef struct {
	volatile struct dbgu_mem_layout* controller;
} dbgu_controller_driver;

int dbgu_init(dbgu_controller_driver* self);

i32 dbgu_put_byte(dbgu_controller_driver* self, const byte c);

i32 dbgu_get_byte(dbgu_controller_driver* self);

void print(char* str);
#endif