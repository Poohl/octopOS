
#include "dbgu.h"

#include "default.h"
#include "interfaces.h"
#include "../memory-map.h"


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

    while(1)
    {
        if(GET32(AUX_MU_LSR_REG)&0x01) break;
    }
    
	return GET32(AUX_MU_IO_REG);
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

int debug_get_char() {
	return dbgu_get_byte();
}


int debug_put_char(char c) {
	return dbgu_put_byte(c);
}