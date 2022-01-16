
#ifndef HALDE_HPP
#define HALDE_HPP

#include "default.hpp"

template <typename T, uint size>
class Halde { // smallest a the top
	private:
		T _buffer[size];
		T* buffer = _buffer - 1;
		uint fill = 0;
		bool (*tLessEq)(const T*, const T*);
		void swap(uint a, uint b) {
			T tmp = buffer[a];
			buffer[a] = buffer[b];
			buffer[b] = tmp;
		};
		void heapifyUp(uint idx) {
			while (idx > 1 && !tLessEq(&buffer[idx / 2], &buffer[idx])) {
				swap(idx, idx/2);
				idx /= 2;
			}
		};
		void heapifyDown(uint idx) {
			while (idx * 2 < fill) {
				uint minChild = idx * 2 == fill ? idx * 2 :
					(tLessEq(&buffer[idx * 2], &buffer[idx * 2 + 1]) ? idx * 2 : idx * 2 + 1);
				if (!tLessEq(&buffer[idx], &buffer[minChild])) {
					swap(minChild, idx);
					idx = minChild;
				} else
					break;
			}
		};
	public:
		Halde(bool (*_tLessEq)(const T*, const T*)) : tLessEq(_tLessEq) {};
		bool put(T* e) {
			if (fill >= size)
				return false;
			buffer[++fill] = *e;
			heapifyUp(fill);
			return true;
		};
		T* peek() {
			return fill > 0 ? &buffer[1] : NULL;
		};
		bool pop() {
			if (fill <= 0)
				return false;
			buffer[1] = buffer[fill--];
			heapifyDown(1);
			return true;
		};
		uint get_full() {
			return fill;
		}
};

#endif
