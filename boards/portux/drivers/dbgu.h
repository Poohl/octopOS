
#ifndef DBGU_H
#define DBGU_H

#include "default.h"
#include "interfaces.h"

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
} if_hw_mem_dbgu;

int dbgu_put_byte(byte c);
int dbgu_init();
int dbgu_get_byte();

sequence_io_status dbgu_write(uint len, const byte* data);
sequence_io_status dbgu_read(uint len, byte* buff);

#endif