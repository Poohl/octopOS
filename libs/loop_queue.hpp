
#ifndef LOOP_QUEUE_HPP
#define LOOP_QUEUE_HPP

#include "default.hpp"

/* here's the limit of c...*/

template <typename T, uint size> class LoopQueue {
	private:
		uint read;
		uint fill;
		T buffer[size];
	public:
		LoopQueue() {
			fill = read = 0;
		}
		uint get_free() {
			return size - fill;
		}
		uint get_full() {
			return fill;
		}
		uint push(const T* data, uint len) {
			uint i;
			for (i = 0; i < len && push(data[i]); ++i);
			return i;
		}
		bool push(T elem) {
			bool out = get_free() > 0;
			if (out)
				buffer[(read + fill++) % size] = elem;
			return out;
		}
		uint pop(T* dest, uint len) {
			uint i;
			T* buff;
			for (i = 0; i < len && (buff = pop()); ++i)
				dest[i] = *buff;
			return i; 
		}
		T* pop() {
			if (fill > 0) {
				--fill;
				return &buffer[read++ % size];
			} else
				return NULL;
		}
		T* peek() {
			if (fill > 0)
				return &buffer[read % size];
			else
				return NULL;
		}
		T* iter(T* c) {
			int pos = c
					? ((c - buffer) % size) - (read % size) + 1
					: (read % size);
			if (pos < 0)
				pos += size;
			return pos < fill ? &buffer[(read + pos) % size] : NULL;
		}
};

#endif
