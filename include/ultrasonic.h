#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <avr/io.h> //access I/O registers
#include <util/delay.h>

#define TRIG_PIN PB4 // pin 10
#define ECHO_PIN PB5 // pin 11

void ultrasiinic_init(void);
int get_distance(void);
void update_led(int distadce);

#endif