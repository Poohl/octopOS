#ifndef INTERFACES_H
#define INTERFACES_H

#include "default.h"

// Utilites, maybe move somewhere else
/**
 * Struct to hold the result of multiple sequential IO operations, as we don't have errno
 * io: number of sucessfull io operations
 * err: error on the (io+1)th operation or 0 if all requests were sucessfull
 */
typedef struct {
	uint io;
	int err;
} sequence_io_status;

#endif
