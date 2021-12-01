
#ifndef UART_H
#define UART_H

#include "default.h"
#include "libs/hardware.h"

int uart_init ( void );
void uart_send ( unsigned int x );
unsigned int uart_recv ( void );
sequence_io_status uart_read(uint len, byte* buff);
sequence_io_status uart_write(uint len, const byte* data);

#endif