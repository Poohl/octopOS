
#include "dbgu.h"

#include "default.h"
#include "memory-map.h"

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

int dbgu_init(if_hw_mem_dbgu* child) {
	/* I'd expect something like tis to be needed, but I'm not messing up working code
	child->interrupt_disable = STATUS_ALL;
	child->baudrate_generator = 1;
	child->mode = MODE_NORMAL | MODE_PARITY_EVEN;
	child->control = CONTROL_DISABLE_TX | CONTROL_DISABLE_RX;
	*/
	return 0;
}

int dbgu_put_byte(if_hw_mem_dbgu* child, byte c) {
	u32 s = 0;
	while (!(s = (child->status & ~(STATUS_RX_READY))));
	if (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY)) {
		child->control = CONTROL_RESET_STATUS_BITS;
		return s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY);
	}
	child->tx = c;
	return 0;
}

int dbgu_get_byte(if_hw_mem_dbgu* child) {
	u32 s = 0;
	while (!(s = (child->status & ~(STATUS_TX_READY | STATUS_TX_EMPTY))));
	if (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY)) {
		child->control = CONTROL_RESET_STATUS_BITS;
		return - (int) (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY));
	}
	return child->rx;
}

