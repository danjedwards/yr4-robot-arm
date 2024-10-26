#include <avr/io.h>
#include <util/delay.h>

void blink(void)
{
    DDRB |= (1 << PB0); // Set pin 0 on port B as output
    while (1)
    {
        PORTB ^= (1 << PB0); // Toggle the pin
        _delay_ms(1);        // Delay for 500ms
    }
}

int main()
{
    blink();
    return 0;
}