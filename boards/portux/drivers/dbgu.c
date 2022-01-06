
#include "dbgu.h"

#include "kernel/syscalls.h"
#include "default.h"
#include "board.h"
#include "libs/hardware.h"
#include "libs/loop_queue.h"
#include "cpu.h"
#include "kernel/process_mgmt.h"

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
static byte_loop_queue recv_buff;

int dbgu_init() {
	dbgu = (if_hw_mem_dbgu*) DBGU;
	dbgu->control = CONTROL_ENABLE_RX | CONTROL_RESET_STATUS_BITS;
	dbgu->interrupt_enable = STATUS_RX_READY;
	blq_init(&send_buff);
	blq_init(&recv_buff);
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

static void slow_print(char c) {
	for (int i = 0; i < c; ++i) {
		sys_debug_put_char(c);
		for (int j = 0; j < 10000000; ++j)
			asm volatile("" : : :);
	}
}

void dbgu_interupt_callback() {
	u32 status = dbgu->status;
	if (status & STATUS_RX_READY) {
		//blq_push(&recv_buff, dbgu->rx);
		init_thread_state_args args = default_init_thread_state_args;
		args.start = (void_void_func_ptr) slow_print;
		args.args[0] = dbgu->rx;
		args.is_sys = true;
		args.stack_size = 0x100;
		char name[] = {'T', 'e', 's', 't', ' ', (char) args.args[0], 0};
		if (new_thread(name, &args) < 0)
			printf("Error creating thread %s\r\n", name);

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
	//printf("shits and giggles\r\n");
	dbgu->interrupt_enable = STATUS_TX_READY;
	//asm volatile("nop\n nop");
	 // return from startup
	//asm volatile("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop" : : : "memory", "lr"); // hard reset cpu & crash gdb
	dbgu_interupt_callback(); // return from _startup
	// Fun fact: if you uncomment the line above, somehow the return from ISR breaks
}

uint dbgu_read_async(uint len, byte* dest) {
	return blq_pop_multi(&recv_buff, dest, len);
}

uint dbgu_async_read_flush() {
	uint out = recv_buff.c_size;
	blq_init(&recv_buff);
	return out;
}

sequence_io_status dbgu_async_write_flush() {
	while (dbgu->interrupt_mask & STATUS_TX_READY) asm volatile("":::);
	sequence_io_status out = {0,0};
	return out;
}

void printyprint() {
	printf("yee\n");
}

byte get_recvbuff_head() {
	while (recv_buff.c_size == 0) asm("":::"memory"); // DON'T TOUCH THIS OR IT BREAKS!!!
	return blq_pop(&recv_buff);
}