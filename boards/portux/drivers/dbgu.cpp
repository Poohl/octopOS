
#include "dbgu.hpp"
#include "libs/loop_queue.hpp"

#define CONTROL_RESET_RX (1 << 2)
#define CONTROL_ENABLE_RX (1 << 4)
#define CONTROL_DISABLE_RX (1 << 5)
#define CONTROL_ENABLE_TX (1 << 6)
#define CONTROL_DISABLE_TX (1 << 7)
#define CONTROL_RESET_STATUS_BITS (1 << 8)

#define MODE_NORMAL (0b00 << 14)
#define MODE_PARITY_EVEN (0b000 << 9)

#define STATUS_RX_READY (1 << 0)
#define STATUS_TX_READY (1 << 1)
#define STATUS_TX_EMPTY (1 << 9)
#define STATUS_ERR_OVERRUN (1 << 5)
#define STATUS_ERR_FRAME (1 << 6)
#define STATUS_ERR_PARITY (1 << 7)

#define STATUS_ALL 0b1100 ## 0000 ## 0000 ## 0000 ## 0001 ## 1010 ## 1111 ## 1011

#define ERR_ABORT (1 << 8)
#define ERR_NO_REQUEST (1 << 9)

DebugUnit::DebugUnit() : base(NULL) {};

DebugUnit::DebugUnit(mmio_dbgu* b) : base(b) {
	base->interrupt_disable = 0xFFFFFFFF;
	base->control = CONTROL_RESET_STATUS_BITS;
}

bool DebugUnit::baseCallback() {
	u32 status = this->base->status;
	if (status & STATUS_RX_READY) {
		if (readQueue.peek()) {
			readQueue.peek()->dest[readProgress++] = (byte) base->rx;
			if (readQueue.peek()->len == readProgress) {
				readQueue.pop()->done->call(sequence_io_status {readProgress, 0});
				readProgress = 0;
			}
		} else
			base->interrupt_disable = STATUS_RX_READY;
	}
	if (status & STATUS_TX_READY) {
		if (writeQueue.peek()) {
			base->tx = writeQueue.peek()->data[writeProgress++];
			if (writeProgress == writeQueue.peek()->len)
				writeQueue.pop()->done->call(sequence_io_status {writeProgress, 0});
				writeProgress = 0;
		} else {
			this->base->interrupt_disable = STATUS_TX_READY;
		}
	}
	return status;
}

void DebugUnit::read(byte* dest, uint len, Callback<sequence_io_status>* done) {
	this->readQueue.push(AsyncReadRequest {dest, len, done});
	this->base->interrupt_enable = STATUS_RX_READY;

}

void DebugUnit::write(const byte* data, uint len, Callback<sequence_io_status>* done) {
	this->writeQueue.push(AsyncWriteRequest {data, len, done});
	this->base->interrupt_enable = STATUS_TX_READY;
}

int DebugUnit::pending() {
	AsyncWriteRequest* c_request = this->writeQueue.peek();
	return  c_request ? c_request->len - this->writeProgress : -1;
}

sequence_io_status DebugUnit::abort() {
	this->base->interrupt_disable = STATUS_TX_READY;
	if (this->writeQueue.peek()) {
		sequence_io_status out = {
			this->pending(),
			ERR_ABORT
		};
		this->writeProgress = 0;
		this->writeQueue.pop();
		if (this->writeQueue.peek())
			this->base->interrupt_enable = STATUS_TX_READY;
		return out;
	} else
		this->base->interrupt_enable = STATUS_TX_READY;
	return {0, 1};
}

int DebugUnit::write_byte(byte data) {
	u32 s = 0;
	while (!(s = (this->base->status & (STATUS_TX_EMPTY | STATUS_TX_READY | STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY))));
	if (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY)) {
		this->base->control = CONTROL_RESET_STATUS_BITS;
		return s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY);
	}
	this->base->tx = data;
	return 0;
}

int DebugUnit::read_byte() {
	u32 s = 0;
	while (!(s = (this->base->status & (STATUS_RX_READY | STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY))));
	if (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY)) {
		this->base->control = CONTROL_RESET_STATUS_BITS;
		return - (int) (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY));
	}
	return this->base->rx;
}
