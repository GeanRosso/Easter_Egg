#include <uart.h>

void uart_init(uint32_t baud) {
    uint32_t ubrr = F_CPU / 16 / baud - 1;
    //high and low bytees of baud rate register, since the calculated number might be larget than 255 (1 byte), split it into 8 bit parts
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;

    //enable receiver and transmiter to ouptup
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    //// set data frame: 8bits, 1 stop
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_putchar(char data) {
    //checks the UART Data Register Empty (UDRE0) flag and if the hardware is busy sending data 
    while (!(UCSR0A & (1 << UDRE0)));
    // once harware is ready, it drops it drop the charachter into UDR0. it does by converting that character into 8 eletrical pulses on the TX pin
    UDR0 = data;
}
//wraper function, so I dont have to send 15 uart_putchar 
void uart_putstr(const char* s) {
    while (*s) uart_putchar(*s++);
}