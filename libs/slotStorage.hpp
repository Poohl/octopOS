
#ifndef SLOT_STORAGE
#define SLOT_STORAGE

#include "loop_queue.hpp"
#include "default.hpp"

template <typename T, uint size>
class SlotStorage {
	private:
		T buffer[size];
		LoopQueue<uint, size> alloc;
	public:
		SlotStorage() {
			for (uint i = 0; i < size; ++i)
				alloc.push(i);
		};
		T* get() {
			int* i = alloc.pop();
			alloc.push(i);
			return i ? &buffer[*i] : NULL;
		};
		void release(T* s) {
			alloc.push(s - buffer);
		};
};

#endif