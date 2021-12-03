
#ifndef UART_H
#define UART_H

#include "default.h"
#include "libs/hardware.h"

int uart_init ( void );
void uart_send ( unsigned int x );
unsigned int uart_recv ( void );
sequence_io_status uart_read(uint len, byte* buff);
sequence_io_status uart_write(uint len, const byte* data);

void uart0_hand();

void uart_write_async(uint len, const byte* data);

uint uart_read_async(uint len, byte* dest);

sequence_io_status uart_async_write_flush();

uint uart_async_read_flush();

#endif