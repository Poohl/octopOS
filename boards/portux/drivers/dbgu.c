
#include "dbgu.h"

#include "default.h"
#include "interfaces.h"
#include "memory-map.h"
#include "libs/hardware.h"

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

int dbgu_init() {
	dbgu = (if_hw_mem_dbgu*) DBGU;
	dbgu->control = CONTROL_ENABLE_RX | CONTROL_RESET_STATUS_BITS;
	return 0;
}

int dbgu_put_byte(byte c) {
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
	return dbgu_put_byte(c);
}

int dbgu_get_byte() {
	u32 s = 0;
	while (!(s = (dbgu->status & (STATUS_RX_READY | STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY))));
	if (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY)) {
		dbgu->control = CONTROL_RESET_STATUS_BITS;
		return - (int) (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY));
	}
	return dbgu->rx;
}

int debug_get_char() {
	return dbgu_get_byte();
}
