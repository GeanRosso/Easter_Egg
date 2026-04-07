#include "ultrasonic.h"

void ultrasonic_init(void) {
    // 2. Set TRIG_PIN as Output, ECHO_PIN as Input
    DDRB |= (1 << TRIG_PIN);  // set bit portb to 1 
    DDRB &= ~(1 << ECHO_PIN);  //set bit 5 portb to 0, &= clear a bi
    DDRA |= 0b00111111; // Set LEDs 22-27 as output
}

int get_distance(void) {
    PORTB &= ~(1 << TRIG_PIN);
    _delay_us(2);
    PORTB |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIG_PIN);

    uint32_t count = 0;
    while (!(PINB & (1 << ECHO_PIN))); // set trig low
    while (PINB & (1 << ECHO_PIN)) {
        count++;
        _delay_us(1);
        if (count > 30000) break; 
    }
    // 340 m/s, sound has to go and come back so /2
    return (int)(count * 0.017);
}

void update_leds(int distance) {
    uint8_t led_mask = 0;
    if (distance > 0 && distance < 60) {
        if (distance < 30) led_mask |= (1 << PA0);
        if (distance < 25) led_mask |= (1 << PA1);
        if (distance < 20) led_mask |= (1 << PA2);
        if (distance < 15) led_mask |= (1 << PA3);
        if (distance < 10) led_mask |= (1 << PA4);
        if (distance < 7)  led_mask |= (1 << PA5);
    }
    PORTA = (PORTA & ~0b00111111) | led_mask;
}
//helper for front end, it is easier to get a number rather than check direcly on the pin
int get_led_count(int distance) {
    if (distance <= 0 || distance >= 30) return 0;
    if (distance < 7)  return 6;
    if (distance < 10) return 5;
    if (distance < 15) return 4;
    if (distance < 20) return 3;
    if (distance < 25) return 2;
    return 1;
}