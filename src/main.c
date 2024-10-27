#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#ifdef DEBUG
    #include "avr8-stub.h"
#endif

void blink(void)
{
    DDRB |= (1 << PB5);
    while (1)
    {
        PORTB ^= (1 << PB5);
    }
}

int main()
{
    #ifdef DEBUG
        debug_init();
        sei();
    #endif

    blink();

    return 0;
}