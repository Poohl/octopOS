
extern "C" {
#include "board.h"
}

#include "uart.hpp"

// please pay close attention to how this is swapped from the interrupts
// also why exaclty do we need this register, can't we just use isr_all ?
#define STATUS_RX_EMPTY (1 << 4)
#define STATUS_TX_FULL (1 << 5)
#define STATUS_RX_FULL (1 << 6)
#define STATUS_TX_EMPTY (1 << 7)

// the documentation is the most unreadable shit. And even better, the official defs are also a mess.

#define ISR_OVERRUN_ERROR (1 << 10)
#define ISR_BREAK_ERROR (1 << 9)
#define ISR_PARITY_ERROR (1 << 8)
#define ISR_FRAME_ERROR (1 << 7)
#define ISR_RX_DROPPED (1 << 6)
#define ISR_TX_EMPTY (1 << 5)
#define ISR_RX_FULL (1 << 4)
#define ISR_MODEM (0xF)

static void PUT32 ( unsigned int dest, unsigned int val) {
	*((volatile u32*) (dest)) = ((u32) (val));
}
static unsigned int GET32 ( unsigned int dest) {
	return *((volatile u32*) (dest));
}
#define RESETS_BASE                 0x4000C000

#define RESETS_RESET_RW        (RESETS_BASE+0x0+0x0000)
#define RESETS_RESET_XOR       (RESETS_BASE+0x0+0x1000)
#define RESETS_RESET_SET       (RESETS_BASE+0x0+0x2000)
#define RESETS_RESET_CLR       (RESETS_BASE+0x0+0x3000)

#define RESETS_WDSEL_RW        (RESETS_BASE+0x4+0x0000)
#define RESETS_WDSEL_XOR       (RESETS_BASE+0x4+0x1000)
#define RESETS_WDSEL_SET       (RESETS_BASE+0x4+0x2000)
#define RESETS_WDSEL_CLR       (RESETS_BASE+0x4+0x3000)

#define RESETS_RESET_DONE_RW   (RESETS_BASE+0x8+0x0000)
#define RESETS_RESET_DONE_XOR  (RESETS_BASE+0x8+0x1000)
#define RESETS_RESET_DONE_SET  (RESETS_BASE+0x8+0x2000)
#define RESETS_RESET_DONE_CLR  (RESETS_BASE+0x8+0x3000)

#define SIO_BASE                    0xD0000000

#define SIO_GPIO_OUT_RW             (SIO_BASE+0x10)
#define SIO_GPIO_OUT_SET            (SIO_BASE+0x14)
#define SIO_GPIO_OUT_CLR            (SIO_BASE+0x18)
#define SIO_GPIO_OUT_XOR            (SIO_BASE+0x1C)

#define SIO_GPIO_OE_RW              (SIO_BASE+0x20)
#define SIO_GPIO_OE_SET             (SIO_BASE+0x24)
#define SIO_GPIO_OE_CLR             (SIO_BASE+0x28)
#define SIO_GPIO_OE_XOR             (SIO_BASE+0x2C)

#define IO_BANK0_BASE               0x40014000

#define IO_BANK0_GPIO0_CTRL_RW      (IO_BANK0_BASE+0x004+0x0000)
#define IO_BANK0_GPIO0_CTRL_XOR     (IO_BANK0_BASE+0x004+0x1000)
#define IO_BANK0_GPIO0_CTRL_SET     (IO_BANK0_BASE+0x004+0x2000)
#define IO_BANK0_GPIO0_CTRL_CLR     (IO_BANK0_BASE+0x004+0x3000)

#define IO_BANK0_GPIO1_CTRL_RW      (IO_BANK0_BASE+0x00C+0x0000)
#define IO_BANK0_GPIO1_CTRL_XOR     (IO_BANK0_BASE+0x00C+0x1000)
#define IO_BANK0_GPIO1_CTRL_SET     (IO_BANK0_BASE+0x00C+0x2000)
#define IO_BANK0_GPIO1_CTRL_CLR     (IO_BANK0_BASE+0x00C+0x3000)

#define IO_BANK0_GPIO25_STATUS_RW   (IO_BANK0_BASE+0x0C8+0x0000)
#define IO_BANK0_GPIO25_STATUS_XOR  (IO_BANK0_BASE+0x0C8+0x1000)
#define IO_BANK0_GPIO25_STATUS_SET  (IO_BANK0_BASE+0x0C8+0x2000)
#define IO_BANK0_GPIO25_STATUS_CLR  (IO_BANK0_BASE+0x0C8+0x3000)

#define IO_BANK0_GPIO25_CTRL_RW     (IO_BANK0_BASE+0x0CC+0x0000)
#define IO_BANK0_GPIO25_CTRL_XOR    (IO_BANK0_BASE+0x0CC+0x1000)
#define IO_BANK0_GPIO25_CTRL_SET    (IO_BANK0_BASE+0x0CC+0x2000)
#define IO_BANK0_GPIO25_CTRL_CLR    (IO_BANK0_BASE+0x0CC+0x3000)

#define CLOCKS_BASE                 0x40008000

#define CLK_REF_CTRL_RW             (CLOCKS_BASE+0x30+0x0000)
#define CLK_REF_CTRL_XOR            (CLOCKS_BASE+0x30+0x1000)
#define CLK_REF_CTRL_SET            (CLOCKS_BASE+0x30+0x2000)
#define CLK_REF_CTRL_CLR            (CLOCKS_BASE+0x30+0x3000)

#define CLK_SYS_CTRL_RW             (CLOCKS_BASE+0x3C+0x0000)
#define CLK_SYS_CTRL_XOR            (CLOCKS_BASE+0x3C+0x1000)
#define CLK_SYS_CTRL_SET            (CLOCKS_BASE+0x3C+0x2000)
#define CLK_SYS_CTRL_CLR            (CLOCKS_BASE+0x3C+0x3000)

#define CLK_PERI_CTRL_RW            (CLOCKS_BASE+0x48+0x0000)
#define CLK_PERI_CTRL_XOR           (CLOCKS_BASE+0x48+0x1000)
#define CLK_PERI_CTRL_SET           (CLOCKS_BASE+0x48+0x2000)
#define CLK_PERI_CTRL_CLR           (CLOCKS_BASE+0x48+0x3000)


#define CLK_SYS_RESUS_CTRL_RW       (CLOCKS_BASE+0x78+0x0000)
#define CLK_SYS_RESUS_CTRL_XOR      (CLOCKS_BASE+0x78+0x1000)
#define CLK_SYS_RESUS_CTRL_SET      (CLOCKS_BASE+0x78+0x2000)
#define CLK_SYS_RESUS_CTRL_CLR      (CLOCKS_BASE+0x78+0x3000)


#define XOSC_BASE                   0x40024000

#define XOSC_CTRL_RW                (XOSC_BASE+0x00+0x0000)
#define XOSC_CTRL_XOR               (XOSC_BASE+0x00+0x1000)
#define XOSC_CTRL_SET               (XOSC_BASE+0x00+0x2000)
#define XOSC_CTRL_CLR               (XOSC_BASE+0x00+0x3000)

#define XOSC_STATUS_RW              (XOSC_BASE+0x04+0x0000)
#define XOSC_STATUS_XOR             (XOSC_BASE+0x04+0x1000)
#define XOSC_STATUS_SET             (XOSC_BASE+0x04+0x2000)
#define XOSC_STATUS_CLR             (XOSC_BASE+0x04+0x3000)

#define XOSC_STARTUP_RW             (XOSC_BASE+0x0C+0x0000)
#define XOSC_STARTUP_XOR            (XOSC_BASE+0x0C+0x1000)
#define XOSC_STARTUP_SET            (XOSC_BASE+0x0C+0x2000)
#define XOSC_STARTUP_CLR            (XOSC_BASE+0x0C+0x3000)

#define UART0_BASE_UARTDR_RW        (UART0_BASE+0x000+0x0000)
#define UART0_BASE_UARTFR_RW        (UART0_BASE+0x018+0x0000)
#define UART0_BASE_UARTIBRD_RW      (UART0_BASE+0x024+0x0000)
#define UART0_BASE_UARTFBRD_RW      (UART0_BASE+0x028+0x0000)
#define UART0_BASE_UARTLCR_H_RW     (UART0_BASE+0x02C+0x0000)
#define UART0_BASE_UARTCR_RW        (UART0_BASE+0x030+0x0000)

#define STK_CSR 0xE000E010
#define STK_RVR 0xE000E014
#define STK_CVR 0xE000E018


static void clock_init (void) {
	PUT32(CLK_SYS_RESUS_CTRL_RW,0);
	//PUT32(CLK_REF_CTRL_RW,0);
	//PUT32(CLK_SYS_CTRL_RW,0);
	PUT32(XOSC_CTRL_RW,0xAA0);      //1 - 15MHZ
	PUT32(XOSC_STARTUP_RW,47);      //straight from the datasheet
	PUT32(XOSC_CTRL_SET,0xFAB000);  //enable
	while(1)
	{
		if((GET32(XOSC_STATUS_RW)&0x80000000)!=0) break;
	}
	PUT32(CLK_REF_CTRL_RW,2); //XOSC
	PUT32(CLK_SYS_CTRL_RW,0); //reset/clk_ref

	PUT32(CLK_PERI_CTRL_RW,(1<<11)|(4<<5));
}

static void reset_init( void ) {
	PUT32(RESETS_RESET_CLR,(1<<5)); //IO_BANK0
	while(1)
	{
		if((GET32(RESETS_RESET_DONE_RW)&(1<<5))!=0) break;
	}
	PUT32(RESETS_RESET_CLR,(1<<22)); //UART0
	while(1)
	{
		if((GET32(RESETS_RESET_DONE_RW)&(1<<22))!=0) break;
	}
}

static void uart_init() {
	//GPIO 0 UART0 TX function 2
	//GPIO 1 UART0 RX function 2

	//(12000000/(16/115200)) = 6.514
	//0.514*64 = 32.666
	PUT32(UART0_BASE_UARTIBRD_RW,6);
	PUT32(UART0_BASE_UARTFBRD_RW,33);
	//0 11 1 0 0 0 0
	//0111 0000
	PUT32(UART0_BASE_UARTLCR_H_RW,0x70);
	PUT32(UART0_BASE_UARTCR_RW,(1<<9)|(1<<8)|(1<<0));
}

static void GPIO_init(void) {
	PUT32(IO_BANK0_GPIO0_CTRL_RW,2);  //UART
	PUT32(IO_BANK0_GPIO1_CTRL_RW,2);  //UART
}

void Uart::init(mmio_uart* _base) {
	clock_init();
	reset_init();
	GPIO_init();
	base = _base;
	base->int_baudrate_divisor = 6;
	base->frac_baudrate_divisor = 33;
	base->mode = 0x70;
	base->control = (1<<9)|(1<<8)|(1<<0);
	*hw_clear_alias(&_base->isr_mask) = 0xFFFFFFFF;
	//*hw_set_alias(&_base->isr_mask) = ISR_RX_FULL | ISR_RX_DROPPED | ISR_OVERRUN_ERROR | ISR_PARITY_ERROR | ISR_BREAK_ERROR | ISR_FRAME_ERROR;
	//*hw_set_alias(&_base->isr_mask) = ISR_OVERRUN_ERROR | ISR_PARITY_ERROR | ISR_BREAK_ERROR | ISR_FRAME_ERROR;
	//while (1)
	//	write_byte('c');
}

bool Uart::baseCallback() {
	u32 status = this->base->status;
	if (status & STATUS_RX_FULL) {
		if (readQueue.peek()) {
			readQueue.peek()->dest[readProgress++] = (byte) base->data;
			if (readQueue.peek()->len == readProgress) {
				readQueue.pop()->done->call(sequence_io_status {readProgress, 0});
				readProgress = 0;
			}
		} else
			*hw_clear_alias(&base->isr_mask) = ISR_RX_FULL;
	}
	if (status & STATUS_TX_EMPTY) {
		if (writeQueue.peek()) {
			base->data = writeQueue.peek()->data[writeProgress++];
			if (writeProgress == writeQueue.peek()->len) {
				Callback<sequence_io_status>* d = writeQueue.pop()->done;
				if (d)
					d->call(sequence_io_status {writeProgress, 0});
				writeProgress = 0;
			}
		} else {
			*hw_clear_alias(&base->isr_mask) = ISR_TX_EMPTY;
		}
	}
	//base->isr_clear = ~0;
	return status;
}

void Uart::read(byte* dest, uint len, Callback<sequence_io_status>* done) {
	this->readQueue.push(AsyncReadRequest {dest, len, done});
	*hw_set_alias(&base->isr_mask) = ISR_RX_FULL;

}

void Uart::write(const byte* data, uint len, Callback<sequence_io_status>* done) {
	this->writeQueue.push(AsyncWriteRequest {data, len, done});
	*hw_set_alias(&base->isr_mask) = ISR_TX_EMPTY;
}

int Uart::pending() {
	AsyncWriteRequest* c_request = this->writeQueue.peek();
	return  c_request ? c_request->len - this->writeProgress : -1;
}

sequence_io_status Uart::abort() {
	*hw_clear_alias(&base->isr_mask) = ISR_TX_EMPTY;
	if (this->writeQueue.peek()) {
		sequence_io_status out = {
			this->pending(),
			ERR_ABORT
		};
		this->writeProgress = 0;
		this->writeQueue.pop();
		if (this->writeQueue.peek())
			*hw_set_alias(&base->isr_mask) = ISR_TX_EMPTY;
		return out;
	} else
		*hw_set_alias(&base->isr_mask) = ISR_TX_EMPTY;
	return {0, 1};
}

int Uart::write_byte(byte data) {
	while (base->status & STATUS_TX_FULL);
	this->base->data = data;
	return 0; // there are no errors in ba-sing-sei
}

int Uart::read_byte() {
	while (this->base->status & STATUS_RX_EMPTY);
	return this->base->data;
}
