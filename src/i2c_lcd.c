#include <i2c_lcd.h>

static uint8_t backlight_state = 0x08; 

void i2c_init(void) {
    TWSR = 0x00; 
    TWBR = 0x48; 
    TWCR = (1<<TWEN);
}

void i2c_packet(uint8_t data) {
    TWDR = data; 
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}

void lcd_send(uint8_t val, uint8_t mode) {
    // We combine the value with the mode and the current backlight state
    uint8_t high = (val & 0xf0) | mode | backlight_state; 
    uint8_t low = ((val << 4) & 0xf0) | mode | backlight_state;
    
    // START
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));

    // ADDRESS
    i2c_packet(LCD_ADDR);

    // DATA HIGH (Pulse Enable)
    i2c_packet(high | 0x04); 
    i2c_packet(high);

    // DATA LOW (Pulse Enable)
    i2c_packet(low | 0x04);  
    i2c_packet(low);

    // STOP
    TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
}

void lcd_init(void) {
    i2c_init();
    _delay_ms(50);
    
    // Force 4-bit mode sequence
    lcd_send(0x03, 0); 
    _delay_ms(5);
    lcd_send(0x03, 0);
    _delay_us(150);
    lcd_send(0x03, 0);
    lcd_send(0x02, 0); 
    
    lcd_send(0x28, 0); // 2 lines, 5x8
    lcd_send(0x06, 0); // Increment cursor
    lcd_send(0x0C, 0); // Display ON
    lcd_clear();
}

void lcd_clear(void) { 
    lcd_send(0x01, 0); 
    _delay_ms(2); 
}

void lcd_putstr(const char* s, uint8_t row) {
    lcd_send(row == 0 ? 0x80 : 0xC0, 0);
    while (*s) lcd_send(*s++, 1);
}

void lcd_backlight(uint8_t state) {
    if (state) backlight_state = 0x08; // Backlight ON bit
    else backlight_state = 0x00;       // Backlight OFF
    
    // Send a dummy command to update the physical backlight
    lcd_send(0x00, 0); 
}

void lcd_display_state(uint8_t state) {
    if (state) lcd_send(0x0C, 0); // Display ON
    else lcd_send(0x08, 0);       // Display OFF
}