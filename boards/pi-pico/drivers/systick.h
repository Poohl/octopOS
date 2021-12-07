
#ifndef SYSTICK_H
#define SYSTICK_H

#include "default.h"

typedef struct {
	u32 control_and_status;
	u32 period;
	u32 value;
	u32 calibration;
} systick;

#endif