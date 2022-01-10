
#ifndef STREAMS_H
#define STREAMS_H
extern "C" {
#include "libs/hardware.h"
}
#include "default.hpp"
#include "Callback.hpp"

class BlockingInStream {
	public:
		virtual sequence_io_status read(byte* dest, uint len);
		virtual int read_byte() = 0;
};

class BlockingOutStream {
	public:
		virtual sequence_io_status write(const byte* data, uint len);
		virtual int write_byte(byte data) = 0;
};

class AsyncInStream {
	public:
		virtual void read(byte* dest, uint len, Callback<sequence_io_status>* done) = 0;
		virtual int available();
		virtual sequence_io_status read_available(byte* dest, uint len);
		virtual sequence_io_status discard();
};

class AsyncOutStream {
	public:
		virtual void write(const byte* data, uint len, Callback<sequence_io_status>* done) = 0;
		virtual int pending();
		virtual sequence_io_status abort();
};

#endif
