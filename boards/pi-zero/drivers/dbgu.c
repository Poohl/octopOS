
#include "dbgu.h"

#include "default.h"
#include "interfaces.h"
#include "../memory-map.h"

/*
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


int dbgu_get_byte() {
	u32 s = 0;
	while (!(s = (dbgu->status & (STATUS_RX_READY | STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY))));
	if (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY)) {
		dbgu->control = CONTROL_RESET_STATUS_BITS;
		return - (int) (s & (STATUS_ERR_OVERRUN | STATUS_ERR_FRAME | STATUS_ERR_PARITY));
	}
	return dbgu->rx;
}
*/
////////////////////
// fanc asm funcs //
////////////////////
extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void dummy ( unsigned int );

////////////////////////
// PI ZERO MEM LAYOUT //
////////////////////////
#define GPFSEL1 0x20200004
#define GPSET0  0x2020001C
#define GPCLR0  0x20200028
#define GPPUD       0x20200094
#define GPPUDCLK0   0x20200098

#define AUX_ENABLES     0x20215004
#define AUX_MU_IO_REG   0x20215040
#define AUX_MU_IER_REG  0x20215044
#define AUX_MU_IIR_REG  0x20215048
#define AUX_MU_LCR_REG  0x2021504C
#define AUX_MU_MCR_REG  0x20215050
#define AUX_MU_LSR_REG  0x20215054
#define AUX_MU_MSR_REG  0x20215058
#define AUX_MU_SCRATCH  0x2021505C
#define AUX_MU_CNTL_REG 0x20215060
#define AUX_MU_STAT_REG 0x20215064
#define AUX_MU_BAUD_REG 0x20215068

/////////////////////////////////////////
// not really dbgu anymore but well... //
/////////////////////////////////////////
int dbgu_put_byte(byte c)
{
    while(1)
    {
        if(GET32(AUX_MU_LSR_REG)&0x20) break;
    }
    PUT32(AUX_MU_IO_REG,c);
	
	return 0;	// lol
}

int dbgu_get_byte()
{
	/* joke's on you shit does nuthin*/
	return 42;
}

sequence_io_status dbgu_write(uint len, const byte* data) {
	sequence_io_status out = {};
	for (out.io = 0; out.io < len &&
			!(out.err = dbgu_put_byte(data[out.io]));
		++out.io);
	if (out.err)
		out.io -= 1;
	return out;
}

/*
sequence_io_status dbgu_read(uint len, byte* buff) {
	sequence_io_status out = {};
	for (out.io = 0; out.io < len &&
			0 <= (out.err = dbgu_get_byte());
		++out.io) {
			buff[out.io] = (byte) out.err;
		}
	if (out.err < 0)
		out.io -= 1;
	else
		out.err = 0;
	return out;
}
*/

int debug_get_char() {
	return dbgu_get_byte();
}


int debug_put_char(char c) {
	return dbgu_put_byte(c);
}