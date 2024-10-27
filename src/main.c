#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "avr8-stub.h"

void blink(void)
{
    DDRB |= (1 << PB5); // Set pin 0 on port B as output
    while (1)
    {
        PORTB ^= (1 << PB5); // Toggle the pin
        // _delay_ms(1);        // Delay for 500ms
    }
}

int main()
{
    debug_init();
    sei();
    // breakpoint();
    blink();
    return 0;
}