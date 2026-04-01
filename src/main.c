#include <avr/io.h> //access I/O registers
#include <util/delay.h>
#include <stdio.h> // C labrary for sprinf

// Standard Arduino Mega (ATmega2560) Clock Speed alrady defined by platformio
#define BAUD 9600                    // Target Serial Speed
#define MYUBRR F_CPU / 16 / BAUD - 1 // Formula to set the Baud Rate register UBRR

#define TRIG_PIN PB4 // pin 10
#define ECHO_PIN PB5 // pin 11

// Function to initialize UART for Serial communication in C
void uart_init(unsigned int ubrr)
{
    // ubrr0 sets the speed
    UBRR0H = (unsigned char)(ubrr >> 8);    // set high ubrr
    UBRR0L = (unsigned char)ubrr;           // set low ubrr
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // Enable receiver and transmitter rx tx
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // set data frame: 8bits, 1 stop
}

// Function to send a character over Serial
void uart_putchar(char data)
{
    // udre0 bit tell if the hardware is ready for anotther character, we poll untill is ready,
    while (!(UCSR0A & (1 << UDRE0)))
        ;        // wait til transmit buffer is empty
    UDR0 = data; // put data into buffer to be sent
}

// Function to send a string over Serial
void uart_putstr(const char *s)
{
    while (*s)
    {
        uart_putchar(*s++);
    }
}

// entry point
int main(void)
{
    //  Initialize Serial
    uart_init(MYUBRR);

    // 2. Set TRIG_PIN as Output, ECHO_PIN as Input
    DDRB |= (1 << TRIG_PIN);  // set bit 4 portb to 1
    DDRB &= ~(1 << ECHO_PIN); // set bit 5 portb to 0, &= clear a bit

    // set leds to output, data direction register portA,0b00111111 sets the first 6 bits to 1 (Output)
    DDRA |= 0b00111111;

    char buffer[50];
    uart_putstr("System Initialized wrote in C\r\n");

    while (1)
    {
        PORTB &= ~(1 << TRIG_PIN); // set trig low
        _delay_us(2);
        PORTB |= (1 << TRIG_PIN); // set trig to high
        _delay_us(10);
        PORTB &= ~(1 << TRIG_PIN); // set trig low again
        // high mean the output pin is 5v whereas low is 0v

        // 4. Measure Echo
        uint32_t count = 0;
        while (!(PINB & (1 << ECHO_PIN)))
            ; // loops until pin goes high
        while (PINB & (1 << ECHO_PIN))
        { // Count while Echo is HIGH increment every second
            count++;
            _delay_us(1);
            if (count > 30000)
                break; // Timeout
        }

        // speed of sound is 340m/s or 0.034cm/s. sound goes and comes back (/)2,
        int distance = count * 0.017;
        // I can use the distance to light led by led
        if (distance == 0 || distance > 60)
        {
            PORTA = ~0b00111111; // all leds are off
        }
        else
        {
            uint8_t led_mask = 0;
            if (distance < 30)
                led_mask |= (1 << PA0); // led 1
            if (distance < 25)
                led_mask |= (1 << PA1); // led 2
            if (distance < 20)
                led_mask |= (1 << PA2); // led 3
            if (distance < 15)
                led_mask |= (1 << PA3); // led 4
            if (distance < 10)
                led_mask |= (1 << PA4); // led 5
            if (distance < 7)
                led_mask |= (1 << PA5); // led 6

            // apply mask, and clear the first 6 bitrs then or them with our mask
            PORTA = (PORTA & ~0b00111111) | led_mask;
        }
        // enables to print current data from sensor
        sprintf(buffer, "Distance: %d cm\r\n", distance);
        uart_putstr(buffer);

        _delay_ms(200);
    }
    return 0;
}