
#include "default.h"
#include "memory-map.h"

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
} dbgu_mem_layout;

typedef struct {
	volatile dbgu_mem_layout* controller;
} dbgu_controller_driver;


#define CONTROL_RESET_RX (1 << 2)
#define CONTROL_ENABLE_RX (1 << 4)
#define CONTROL_DISABLE_RX (1 << 5)
#define CONTROL_ENABLE_TX (1 << 6)
#define CONTROL_DISABLE_TX (1 << 7)
#define CONTROL_RESET_STATUS_BITS (1 << 8)

#define MODE_NORMAL (0b00 << 14)
#define MODE_PARITY_EVEN (0b000 << 9)

#define STATUS_RX_READY_RX (1 << 0)
#define STATUS_TX_READY (1 << 1)
#define STATUS_TX_EMPTY (1 << 9)

#define STATUS_ALL 0b1100 0000 0000 0000 0001 1010 1111 1011

// at least this works
void tst() {
	dbgu_controller_driver dbgu_cd;
	dbgu_cd.controller = (dbgu_mem_layout*) DBGU;
	dbgu_cd.controller->control = CONTROL_ENABLE_TX;
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = 'h';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = 'e';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = 'l';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = 'l';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = 'o';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = ' ';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = 'w';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = 'o';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = 'r';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = 'l';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = 'd';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = '!';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = ' ';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = ':';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = ')';
	while (!dbgu_cd.controller->status)	{}
	dbgu_cd.controller->tx = '\n';

}

/*
// dont know why the fuck this doesnt work
// it does if int i is a const
// why? WHY?!?!?!

void print(char* str) {
	dbgu_controller_driver dbgu_cd;
	dbgu_cd.controller = (dbgu_mem_layout*) DBGU;
	dbgu_cd.controller->control = CONTROL_ENABLE_TX;
	
	for (i ; str[i] != '\0'; i++) {
		while (! dbgu_cd.controller->status) {}
		dbgu_cd.controller->tx = str[i];
	}

} 
*/