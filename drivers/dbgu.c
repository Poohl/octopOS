
#include "dbgu.h"

#include "default.h"
#include "memory-map.h"

// see page 330 of https://mycampus.imp.fu-berlin.de/access/content/group/90533309-52c5-4d21-b65c-97b90b130b6c/AT91RM9200.pdf
struct dbgu_mem_layout {
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
};

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

int dbgu_init(dbgu_controller_driver* self) {
	/* I'd expect something like tis to be needed, but I'm not messing up working code
	self->controller->interrupt_disable = STATUS_ALL;
	self->controller->baudrate_generator = 1;
	self->controller->mode = MODE_NORMAL | MODE_PARITY_EVEN;
	self->controller->control = CONTROL_DISABLE_TX | CONTROL_DISABLE_RX;
	*/
	return 0;
}

i32 dbgu_put_byte(dbgu_controller_driver* self, const byte c) {
	u32 s = 0;
	while (!(s = (self->controller->status & ~(STATUS_RX_READY))));
	if (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY)) {
		self->controller->control = CONTROL_RESET_STATUS_BITS;
		return s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY);
	}
	self->controller->tx = c;
	return 0;
}

i32 dbgu_get_byte(dbgu_controller_driver* self) {
	u32 s = 0;
	while (!(s = (self->controller->status & ~(STATUS_TX_READY | STATUS_TX_EMPTY))));
	if (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY)) {
		self->controller->control = CONTROL_RESET_STATUS_BITS;
		return - (int) (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY));
	}
	return self->controller->rx;
}

/* simple print */
void print(char* str) {
	/* init dbgu controller */
	dbgu_controller_driver dbgu_cd;
	dbgu_cd.controller = (struct dbgu_mem_layout*) DBGU;
	dbgu_cd.controller->control = CONTROL_ENABLE_TX;
	
	/* print str */
	for (int i = 0; str[i] != '\0'; i++) {
		while (! dbgu_cd.controller->status) {}
		dbgu_cd.controller->tx = str[i];
	}

} 
