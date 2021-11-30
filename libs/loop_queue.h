
#include "default.h"

/* here's the limit of c...*/


typedef struct {
	uint buf_size;
	uint c_size;
	uint write;
	uint read;
	byte buffer[];
} byte_loop_queue;

uint blq_get_space(byte_loop_queue* self);
int blq_push_multi(byte_loop_queue* self, const byte* data, uint len);
bool blq_push(byte_loop_queue* self, byte data);
int blq_pop_multi(byte_loop_queue* self, byte* dest, uint len);
int blq_pop(byte_loop_queue* self);