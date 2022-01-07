
#ifndef STREAMS_H
#define STREAMS_H

#include "default.hpp"
#include "hardware.h"

class BlockingInStream {
	public:
		sequence_io_status read(byte* dest, uint len);
		int read_byte();
};

class BlockingOutStream {
	public:
		sequence_io_status write(const byte* data, uint len);
		int write_byte(byte data);
};

class BlockingIOStream : public BlockingInStream, public BlockingOutStream {
};

class AsyncInStream : public BlockingInStream {
	public:
		void read(byte* dest, uint len, void (*finished_callback)(sequence_io_status*));
		void read_byte(void (*finished_callback)(int));
		int available();
		sequence_io_status read_available(byte* dest, uint len);
		sequence_io_status discard();
};

class AsyncOutStream : public BlockingOutStream {
	public:
		void write(const byte* data, uint len, void (*finished_callback)(sequence_io_status*));
		void write_byte(void (*finished_callback)(int));
		int pending();
		sequence_io_status flush();
		sequence_io_status abort();
};

class AsyncIOStream: public AsyncInStream, public AsyncOutStream {
	
};

#endif
