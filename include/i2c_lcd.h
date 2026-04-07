#ifndef I2C_LCD_H
#define I2C_LCD_H

#include <avr/io.h>
#include <util/delay.h>

#define LCD_ADDR (0x27 << 1)

void lcd_init(void);
void lcd_putstr(const char* s, uint8_t row);
void lcd_clear(void);

#endif