#include "uart.h"
#include "ultrasonic.h"
#include "i2c_lcd.h"

int main(void)
{
    uart_init(9600);
    ultrasonic_init();
    lcd_init();

    char buffer[50];

    //needed to track a state in order to manipulate the power of the lcd
    int system_active = 0;

    while (1)
    {
        // UCSR0A sensor that detects if a new bite has arrived and RXC0 iReceveied complete flag
        // is there letter waiting in the mailbox?
        if (UCSR0A & (1 << RXC0))
        {
            // UART data register  0, all the letters comming from the api drop here
            char received = UDR0;

            //Case U, unnlock, when password is correct
            if (received == 'U' && system_active == 1)
            {
                lcd_clear();
                lcd_putstr("ACCESS GRANTED", 0);
                lcd_putstr("WELCOME HOME", 1);
                // light all the led on all the 6 pins in portA
                PORTA = 0b00111111;
                _delay_ms(3000);

                system_active = 2;
                continue;
            }
            //Case F, wrong password
            else if (received == 'F')
            { 
                lcd_clear();
                lcd_putstr("WRONG PASSWORD", 0);
                lcd_putstr("TRY AGAIN...", 1);

                // Visual feedback: Flash only 2 LEDs as a warning
                PORTA = 0b00000011;
                _delay_ms(2000);
                // Turn all 6 back on for the prompt
                PORTA = 0b00111111; 

                // Rewrite the original prompt
                lcd_clear();
                lcd_putstr("SYSTEM ONLINE", 0);
                lcd_putstr("ENTER PASSWORD:", 1);
            }
            //Case P, panick, shit is going down
            else if (received == 'P')
            {
                lcd_clear();
                lcd_putstr("!!! PANIC !!!", 0);
                lcd_putstr("EXPLODING...", 1);
                // Rapidly blink all LEDs, for 10 seconds
                for (int i = 0; i < 10; i++)
                {
                    PORTA = 0b00111111;
                    _delay_ms(100);
                    PORTA = 0;
                    _delay_ms(100);
                }
                lcd_clear();
                lcd_putstr("System OFFLINE", 0);
                lcd_putstr("RECOVERING.....", 1);

                for (int i = 0; i < 10; i++)
                {
                    PORTA = 0b00101010;
                    _delay_ms(250);
                    PORTA = 0b00010101;
                    _delay_ms(250);
                }

                // The reset, so I dont have to restart physically the arduino, this helps cleaning the lcd once the user fails to input right password
                lcd_clear();
                lcd_backlight(0);
                system_active = 0;
            }
        }

        int distance = get_distance();
        update_leds(distance);

        //check to power the lcd
        if (distance < 7 && distance > 0)
        {
            // if it is off
            if (system_active == 0)
            {   
                //turn on backlight and set state on lcd to output
                lcd_backlight(1);
                lcd_display_state(1);
                lcd_clear();
                lcd_putstr("SYSTEM ONLINE", 0);
                _delay_ms(500);
                lcd_putstr("ENTER PASSWORD:", 1);
                system_active = 1; // Lock it so it doesn't repeat
            }
        }
        // Reset lcd if distance is greater than 15
        else if (distance > 15)
        {
            if (system_active != 0)
            {
                lcd_clear();
                lcd_backlight(0);
                lcd_display_state(0);
                PORTA = 0;
                system_active = 0; // Unlock for next time
            }
        }
        //control the leds
        int current_leds = get_led_count(distance);
        update_leds(distance);   
        //take the number from the sensor and wrap them in json                                                 // Physical
        sprintf(buffer, "{\"leds\":%d,\"dist\":%d}\r\n", current_leds, distance); 
        //send the string out through the usb cable, this is what the web api is catching on the other side
        uart_putstr(buffer);
        //ensure only send data 5 times per second, prevent the arduino sending thousand times per second floading the serial port and crash webapi
        _delay_ms(200);
    }
    return 0;
}