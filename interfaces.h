#ifndef INTERFACES_H
#define INTERFACES_H

#include "default.h"

// Utilites, maybe move somewhere else
typedef struct {
	uint io;
	int err;
} sequence_io_status;

////////////////////////////////////////////////////
// Actual and immuateble harware interfaces if_hw //
////////////////////////////////////////////////////

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

//////////////////////////////////////////////////
// Software driver interfaces if_drv
//////////////////////////////////////////////

/*
	Structure for these is always the same:
	- a pointer to the underlying layer
	- a bunch of functionpointers to the implementations
	so basically vtables

	Note that in the current implemetnaion, the driver does not actully decide how it's own interface is used.
*/

typedef struct {
	void* byte_device;
	int (*put_byte)(void*, byte);
} if_drv_byte_outstream;

typedef struct {
	void* byte_device;
	int (*get_byte)(void*);
} if_drv_byte_instream;

typedef struct {
	void* outstream;
	sequence_io_status (*write)(void*, uint, const byte*);
} if_drv_outsteam;

typedef struct {
	void* instream;
	sequence_io_status (*read)(void*, uint, byte*);
} if_drv_insteam;

#endif