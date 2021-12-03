
#include "dbgu.h"

#include "default.h"
#include "memory-map.h"
#include "libs/hardware.h"
#include "libs/loop_queue.h"

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


static volatile if_hw_mem_dbgu* dbgu;

static byte_loop_queue send_buff;
static volatile byte _send_buff[256];
static byte_loop_queue recv_buff;
static volatile byte _recv_buff[256];

int dbgu_init() {
	dbgu = (if_hw_mem_dbgu*) DBGU;
	dbgu->control = CONTROL_ENABLE_RX | CONTROL_RESET_STATUS_BITS;
	dbgu->interrupt_enable = STATUS_RX_READY;
	byte_loop_queue _lovely_c_spec = {
		256,0,0,0
	};
	send_buff = recv_buff = _lovely_c_spec;
	return 0;
}

int dbgu_put_byte_blocking(byte c) {
	u32 s = 0;
	while (!(s = (dbgu->status & (STATUS_TX_EMPTY | STATUS_TX_READY | STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY))));
	if (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY)) {
		dbgu->control = CONTROL_RESET_STATUS_BITS;
		return s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY);
	}
	dbgu->tx = c;
	return 0;
}

int debug_put_char(char c) {
	return dbgu_put_byte_blocking(c);
}

int dbgu_get_byte_blocking() {
	u32 s = 0;
	while (!(s = (dbgu->status & (STATUS_RX_READY | STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY))));
	if (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY)) {
		dbgu->control = CONTROL_RESET_STATUS_BITS;
		return - (int) (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY));
	}
	return dbgu->rx;
}

int debug_get_char() {
	return dbgu_get_byte_blocking();
}

void dbgu_interupt_callback() {
	u32 status = dbgu->status;
	if (status & STATUS_RX_READY) {
		blq_push(&recv_buff, dbgu->rx);
	}
	if (status & STATUS_TX_READY) {
		int out = blq_pop(&send_buff);
		if (out >= 0) {
			dbgu->tx = out;
		} else {
			dbgu->interrupt_disable = STATUS_TX_READY;
		}
	}
}

void dbgu_write_async(uint len, const byte* data) {
	blq_push_multi(&send_buff, data, len);
	dbgu->interrupt_enable = STATUS_TX_READY;
	dbgu_interupt_callback();
}

uint dbgu_read_async(uint len, byte* dest) {
	return blq_pop_multi(&recv_buff, dest, len);
}

uint dbgu_async_read_flush() {
	uint out = recv_buff.c_size;
	byte_loop_queue _lovely_c_spec = {
		256,0,0,0
	};
	recv_buff = _lovely_c_spec;
	return out;
}

sequence_io_status dbgu_async_write_flush() {
	while (dbgu->interrupt_mask & STATUS_TX_READY);
	sequence_io_status out = {0,0};
	return out;
}

byte get_recvbuff_head() {
	while (recv_buff.c_size == 0) asm("":::"memory"); // DON'T TOUCH THIS OR IT BREAKS!!!
	return blq_pop(&recv_buff);
}