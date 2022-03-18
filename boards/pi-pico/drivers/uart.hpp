
#ifndef UART_HPP
#define UART_HPP
extern "C" {
#include "board.h"
}
#include "default.hpp"
#include "libs/loop_queue.hpp"
#include "drivers/Streams.hpp"

#define ERR_ABORT

typedef struct {
	volatile u32 /* 0x00 */ data;
	volatile u32 /* 0x04 */ error;
	volatile u32 /* 0x08 */ _reserved1[4];
	volatile u32 /* 0x0 */ status;
	volatile u32 /* 0x00 */ _reserved2;
	volatile u32 /* 0x00 */ low_power_counter;
	volatile u32 /* 0x24 */ int_baudrate_divisor;
	volatile u32 /* 0x28 */ frac_baudrate_divisor;
	volatile u32 /* 0x2c */ mode;
	volatile u32 /* 0x30 */ control;
	volatile u32 /* 0x00 */ fifo_isr_level;
	volatile u32 /* 0x00 */ isr_mask;
	volatile u32 /* 0x00 */ isr_all_status;
	volatile u32 /* 0x00 */ isr_masked_status;
	volatile u32 /* 0x00 */ isr_clear;
	volatile u32 /* 0x00 */ dma_control;
	volatile u32 /* 0x00 */ _reserved3[(0xfe0 - 0x48) / 4 - 1];
	volatile u32 /* 0x00 */ preiphid[4];
	volatile u32 /* 0x00 */ cellid[4];
} mmio_uart;

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

class Uart : public AsyncInStream, public AsyncOutStream, public BlockingInStream, public BlockingOutStream {
	private:
		mmio_uart* base;
		LoopQueue<AsyncReadRequest, 16> readQueue;
		LoopQueue<AsyncWriteRequest, 16> writeQueue;
		uint readProgress = 0;
		uint writeProgress = 0;
	public:
		void init(mmio_uart* _base);
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