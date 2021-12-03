#include "default.h"
#include "libs/hardware.h"
#include "libs/loop_queue.h"
#include "board.h"
#include "libs/printf.h"

void PUT32 ( unsigned int dest, unsigned int val) {
	*((volatile u32*) (dest)) = ((u32) (val));
}
unsigned int GET32 ( unsigned int dest) {
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

#define UART0_BASE                  0x40034000

typedef struct {
	u32 data;
	u32 error;
	u32 _reserved1[4];
	u32 status;
	u32 _reserved2;
	u32 low_power_counter;
	u32 int_baudrate_divisor;
	u32 frac_baudrate_divisor;
	u32 mode;
	u32 control;
	u32 fifo_isr_level;
	u32 isr_mask;
	u32 isr_all_status;
	u32 isr_masked_status;
	u32 isr_clear;
	u32 dma_control;
	u32 _reserved3[(0xfe0 - 0x48) / 4 - 1];
	u32 preiphid[4];
	u32 cellid[4];
} uart;

static volatile uart* uart0 = (uart*) UART0_BASE;

static byte_loop_queue send_buff;
static byte_loop_queue recv_buff;

#define UART0_BASE_UARTDR_RW        (UART0_BASE+0x000+0x0000)
#define UART0_BASE_UARTFR_RW        (UART0_BASE+0x018+0x0000)
#define UART0_BASE_UARTIBRD_RW      (UART0_BASE+0x024+0x0000)
#define UART0_BASE_UARTFBRD_RW      (UART0_BASE+0x028+0x0000)
#define UART0_BASE_UARTLCR_H_RW     (UART0_BASE+0x02C+0x0000)
#define UART0_BASE_UARTCR_RW        (UART0_BASE+0x030+0x0000)

#define STK_CSR 0xE000E010
#define STK_RVR 0xE000E014
#define STK_CVR 0xE000E018

// please pay close attention to how this is swapped from the interrupts
// also why exaclty do we need this register, can't we just use isr_all ?
#define STATUS_RX_EMPTY (1 << 4)
#define STATUS_TX_FULL (1 << 5)

// the documentation is the most unreadable shit. And even better, the official defs are also a mess.

#define ISR_OVERRUN_ERROR (1 << 10)
#define ISR_BREAK_ERROR (1 << 9)
#define ISR_PARITY_ERROR (1 << 8)
#define ISR_FRAME_ERROR (1 << 7)
#define ISR_RX_DROPPED (1 << 6)
#define ISR_TX_EMPTY (1 << 5)
#define ISR_RX_FULL (1 << 4)
#define ISR_MODEM (0xF)

int uart_recv () {
	while(uart0->status & STATUS_RX_EMPTY);
	return uart0->data;
}

int uart_send ( unsigned int x ) {
	while(uart0->status & STATUS_TX_FULL);
	uart0->data = x;
	return 0;
}

void uart0_hand() {
	u32 status = uart0->status;
	//printf(">u0 %x %x\r\n", uart0->isr_all_status, uart0->isr_masked_status);
	
	if (!(status & STATUS_TX_FULL)) {
		uart0->data = blq_pop(&send_buff);
		if (send_buff.c_size <= 0)
			*hw_clear_alias(&uart0->isr_mask) = ISR_TX_EMPTY;
	}
	if (!(status & STATUS_RX_EMPTY)) {
		blq_push(&recv_buff, (byte) uart0->data);
	}
	//printf("u0> %x %x\r\n", uart0->isr_all_status, uart0->isr_masked_status);
}

sequence_io_status uart_write(uint len, const byte* data) {
	sequence_io_status out = {};
	for (out.io = 0; out.io < len &&
			!(out.err = uart_send(data[out.io]));
		++out.io);
	if (out.err)
		out.io -= 1;
	return out;
}

sequence_io_status uart_read(uint len, byte* buff) {
	sequence_io_status out = {};
	for (out.io = 0; out.io < len &&
			0 <= (out.err = uart_recv());
		++out.io) {
			buff[out.io] = (byte) out.err;
		}
	if (out.err < 0)
		out.io -= 1;
	else
		out.err = 0;
	return out;
}

//static void uart_flush ( void )
//{
	//while(1)
	//{
		//if((GET32(UART0_BASE_UARTFR_RW)&(1<<7))!=0) break;
	//}
//}

static void clock_init ( void )
{
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
}

int uart_init ( void )
{

	clock_init();

	PUT32(CLK_PERI_CTRL_RW,(1<<11)|(4<<5));

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

	PUT32(IO_BANK0_GPIO0_CTRL_RW,2);  //UART
	PUT32(IO_BANK0_GPIO1_CTRL_RW,2);  //UART

//	*hw_clear_alias(&uart0->isr_mask) = ISR_RX;
	//uart0->isr_clear = 0xFFFFFFFF;
	recv_buff.buf_size = 256;
	recv_buff.c_size = 0;
	recv_buff.read = 0;
	recv_buff.write = 0;
	send_buff.buf_size = 256;
	send_buff.c_size = 0;
	send_buff.read = 0;
	send_buff.write = 0;
	
	*hw_clear_alias(&uart0->isr_mask) = 0xFFFFFFFF;
	*hw_set_alias(&uart0->isr_mask) = ISR_RX_FULL | ISR_RX_DROPPED | ISR_OVERRUN_ERROR | ISR_PARITY_ERROR | ISR_BREAK_ERROR | ISR_FRAME_ERROR;
	//uart0->isr_mask = 
	return(0);
}

int debug_put_char(char c) {
	return uart_send(c);
}
int debug_get_char() {
	return uart_recv();
}

void uart_write_async(uint len, const byte* data) {
	blq_push_multi(&send_buff, data, len);
	/* 
	 * Yes there'a a race condition here:
	 * If the handler checks the queue, finds it empty
	 * we push & enable interrupts again
	 * then the handler disables them again.
	 * but currently the handler cannot be interrupted.
	 */
	*hw_set_alias(&uart0->isr_mask) = ISR_TX_EMPTY;
}

uint uart_read_async(uint len, byte* dest) {
	return blq_pop_multi(&recv_buff, dest, len);
}

sequence_io_status uart_async_write_flush() {
	while (send_buff.c_size > 0)
		asm volatile ("" : : : );
	sequence_io_status out = {0,0};
	return out;
}

uint uart_async_read_flush() {
	uint out = recv_buff.c_size;
	recv_buff.buf_size = 256;
	recv_buff.c_size = 0;
	recv_buff.read = 0;
	recv_buff.write = 0;
	return out;
}
