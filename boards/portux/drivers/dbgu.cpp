extern "C" {

#include "dbgu.h"

#include "default.h"
#include "board.h"
#include "libs/hardware.h"
}
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


static volatile if_hw_mem_dbgu* dbgu;

static LoopQueue<byte, 256> send_buff;
static LoopQueue<byte, 256> recv_buff;

extern "C" {

int dbgu_init() {
	dbgu = (if_hw_mem_dbgu*) DBGU;
	dbgu->control = CONTROL_ENABLE_RX | CONTROL_RESET_STATUS_BITS;
	dbgu->interrupt_enable = STATUS_RX_READY;
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
		recv_buff.push(dbgu->rx);
	}
	if (status & STATUS_TX_READY) {
		byte* out = send_buff.pop();
		if (out) {
			dbgu->tx = *out;
		} else {
			dbgu->interrupt_disable = STATUS_TX_READY;
		}
	}
}

void dbgu_write_async(uint len, const byte* data) {
	send_buff.push(data, len);
	//printf("shits and giggles\r\n");
	dbgu->interrupt_enable = STATUS_TX_READY;
	//asm volatile("nop\n nop");
	 // return from startup
	//asm volatile("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop" : : : "memory", "lr"); // hard reset cpu & crash gdb
	dbgu_interupt_callback(); // return from _startup
	// Fun fact: if you uncomment the line above, somehow the return from ISR breaks
}

uint dbgu_read_async(uint len, byte* dest) {
	return recv_buff.pop(dest, len);
}

uint dbgu_async_read_flush() {
	uint out = recv_buff.get_space();
	recv_buff = LoopQueue<byte, 256>();
	return out;
}

sequence_io_status dbgu_async_write_flush() {
	while (dbgu->interrupt_mask & STATUS_TX_READY) asm volatile("":::);
	sequence_io_status out = {0,0};
	return out;
}

byte get_recvbuff_head() {
	while (recv_buff.get_space() == 0) asm("":::"memory"); // DON'T TOUCH THIS OR IT BREAKS!!!
	return *recv_buff.pop();
}

}
