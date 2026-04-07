#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stdio.h>

void uart_init(uint32_t baud);
void uart_putstr(const char* s);

#endif