
#ifndef DBGU_H
#define DBGU_H

#include "default.hpp"
#include "libs/loop_queue.hpp"
#include "drivers/Streams.hpp"

struct AsyncReadRequest {
	byte* dest;
	uint len;
	Callback<sequence_io_status>* done;
};

struct AsyncWriteRequest {
	const byte* data;
	uint len;
	Callback<sequence_io_status>* done;
};

// see page 330 of https://mycampus.imp.fu-berlin.de/access/content/group/90533309-52c5-4d21-b65c-97b90b130b6c/AT91RM9200.pdf
typedef struct {
	u32 control;
	u32 mode;
	u32 interrupt_enable;
	u32 interrupt_disable;
	u32 interrupt_mask;
	u32 status;
	u32 rx;
	u32 tx;
	u32 baudrate_generator;
	byte _reserved1[0x3C-0x24+sizeof(u32)];
	u32 chip_id;
	u32 chip_id_extension;
	byte _reserved2[0x4C-0x48];
	byte _reserved3[0xFC-0x4C+sizeof(u32)];
	byte pdc[0x124-0x100+sizeof(u32)];
} mmio_dbgu;

class DebugUnit : public AsyncInStream, public AsyncOutStream, public BlockingInStream, public BlockingOutStream {
	private:
		volatile mmio_dbgu* base;
		LoopQueue<AsyncReadRequest, 16> readQueue;
		LoopQueue<AsyncWriteRequest, 16> writeQueue;
		uint readProgress = 0;
		uint writeProgress = 0;
	public:
		void init(mmio_dbgu* _base);
		virtual void read(byte* dest, uint len, Callback<sequence_io_status>* done);

		virtual void write(const byte* data, uint len, Callback<sequence_io_status>* done);
		virtual int pending();
		virtual sequence_io_status abort();

		virtual int write_byte(byte data);

		virtual int read_byte();

	/* protected: */
		bool baseCallback();
};

#endif