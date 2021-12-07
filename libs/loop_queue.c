
#include "default.h"

#include "loop_queue.h"

/* here's the limit of c...*/

typedef byte_loop_queue blq;

void blq_init(blq* self) {
	self->c_size = self->write = self->read = 0;
}

uint blq_get_space(blq* self) {
	return 0x100 - self->c_size;
}

int blq_push_multi(blq* self, const byte* data, uint len) {
	uint i;
	for (i = 0; i < len && blq_push(self, data[i]); ++i);
	return i;
}

bool blq_push(blq* self, byte data) {
	if (self->c_size < 0x100) {
		self->buffer[(self->write = (self->write + 1) & 0xFF)] = data;
		self->c_size += 1;
		return true;
	} else {
		return false;
	}
}

int blq_pop_multi(blq* self, byte* dest, uint len) {
	uint i;
	int p;
	for (i = 0; i < len && (p = blq_pop(self)) >= 0; ++i)
		dest[i] = (byte) p;
	return i;
}

int blq_pop(blq* self) {
	if (self->c_size > 0) {
		self->c_size -= 1;
		return self->buffer[self->read = ((self->read + 1) & 0xFF)];
	} else
		return -1;
}