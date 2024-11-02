#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#ifdef DEBUG
#include "avr8-stub.h"
#endif

void blink()
{
    DDRB |= (1 << PB5);
    for (int i = 0; i < 20; i++)
    {
        PORTB ^= (1 << PB5);
        _delay_ms(50);
    }
    // PORTB &= ~(1<<PB5);
}

void clk_init(void)
{
    /* Clock Initialisation
     *** System Clocks
     * CPU Clock
     * I/O Clock
     * Flash Clock
     * Asynchronous Timer Clock
     * ADC Clock
     *
     *** Defualt Fuse Settings
     * The low fuse byte is configured as 0xFF (16 MHz undivided external clock).
     * Other fuse bytes are not applicable to clock generation.
     *
     *** Registers
     ** 8.12.1 OSCCAL – Oscillator Calibration Register (0x66)
     * The oscillator calibration register is used to trim the calibrated internal RC oscillator to remove process variations from the
     * oscillator frequency.
     *
     * Since the RC oscillator is not used in the project, the register is not modified from the default register.
     *
     * NB - that this oscillator is used to time EEPROM and flash write accesses, and these write times will be affected accordingly.
     * If the EEPROM or flash are written, do not calibrate to more than 8.8MHz. Otherwise, the EEPROM or flash write may fail.
     *
     ** 8.12.2 CLKPR – Clock Prescale Register (0x61)
     * By default, set to 0b0000 (i.e., scale of 1).
     */

    CLKPR = (1 << CLKPCE); // Clock Prescaler Enable Change. CLKPCE will be set to 0 four clock cycles after being set.
    CLKPR = 0;             // Clock Prescaler Select Bits.
}

void usart_init(void)
{
/** USART Initialisation
 *
 *
 *** Registers
 ** 19.10.1 UDRn – USART I/O Data Register n
 * Ignored during initialisation.
 *
 ** 19.10.2 UCSRnA – USART Control and Status Register n A
 * Contains status information relating to USART. Bits 0 and 1 contrl USART mode.
 *
 ** 19.10.3 UCSRnB – USART Control and Status Register n B
 * Interupts over USART, enable tx/rx and 9th bits of tx/rx buffers.
 *
 ** 19.10.4 UCSRnC – USART Control and Status Register n C
 * USART mode, parity mode, etc.
 *
 ** 19.10.5 UBRRnL and UBRRnH – USART Baud Rate Registers
 * UBRRn = F_OSC/(16*BAUD) - 1
 */

// The serial port is used as a transport protocol for debugging. To avoid affecting debugging, do not change usart settings.
#ifdef DEBUG
    return;
#endif

    // Enable transmitter and receiver
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    // Set frame format: 8 data bits, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Given F_OSC = 16 MHz and a target BAUD of 115200, UBR = 7.68
    UBRR0H = 0;
    UBRR0L = 0b1100111;
}

void usart_flush(void)
{
#ifdef DEBUG
    return; // serial disabled in debug mode
#endif

    unsigned char dummy;
    while (UCSR0A & (1 << RXC0))
        dummy = UDR0;
}

void usart_tx(char *msg, int msg_len)
{
#ifdef DEBUG
    return; // usart disabled in debug mode
#endif

    for (int i = 0; i < msg_len; i++)
    {
        if (msg[i] == '\0')
        {
            return;
        }

        // Wait for empty transmit buffer
        while (!(UCSR0A & (1 << UDRE0)))
            ;

        // Put data into buffer
        UDR0 = msg[i];
    }
    _delay_ms(200);
}

char usart_rx(void)
{
#ifdef DEBUG
    return 0;
#endif

    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)))
        ;

    // Check flags
    if (UCSR0A & (1 << FE0))
    {
        blink();
    }

    // Get and return received data from buffer
    return UDR0;
}

void usart_read_string(char *buffer, uint8_t max_length)
{
    uint8_t i = 0;
    char received_char;

    while (i < max_length - 1)
    {
        received_char = usart_rx();

        if (received_char == '\n' || received_char == '\r')
        {
            break;
        }

        buffer[i++] = received_char;
    }

    buffer[i] = '\0';
}

void interrupt_init()
{
    /** Interrupt Initialisation
     *** External Interrupt Capable Pins
     * All.
     *
     *** Registers
     ** 11.2.2 MCUCR – MCU Control Register
     * Controls where interrupt service routines are placed in memory. Left as default (0x00 + offset).
     *
     ** 12.2.1 EICRA – External Interrupt Control Register A
     * Controls how external interrupts are triggered.
     *
     ** 12.2.2 EIMSK – External Interrupt Mask Register
     * External interrupt enable/disable.
     *
     ** 12.2.3 EIFR – External Interrupt Flag Register
     * External interrupt enable/disable.
     *
     ** 12.2.4 PCICR – Pin Change Interrupt Control Register
     * External interrupt enable/disable.
     *
     ** 12.2.5 PCIFR – Pin Change Interrupt Flag Register
     * Pin change flags.
     *
     ** 12.2.6 PCMSK2 – Pin Change Mask Register 2
     * Enables/disables interrupts on specific pins
     *
     ** 12.2.7 PCMSK1 – Pin Change Mask Register 1
     * Enables/disables interrupts on specific pins
     *
     ** 12.2.8 PCMSK0 – Pin Change Mask Register 0
     * Enables/disables interrupts on specific pins
     */

    // Set PD7 as input with internal pull-up enabled
    DDRD &= ~(1 << DDD7);   // Set PD7 as input
    PORTD |= (1 << PORTD7); // Enable pull-up on PD7

    // Enable Pin Change Interrupt on PCINT23 (PD7)
    PCICR |= (1 << PCIE2);    // Enable Pin Change Interrupt 2 (PCINT[23:16])
    PCMSK2 |= (1 << PCINT23); // Enable interrupt specifically for PCINT23

    // Enable global interrupts
    sei();
}

// Interrupt Service Routine for Pin Change Interrupt 2
ISR(PCINT2_vect)
{
    DDRB |= (1 << PB5);
    PORTB ^= (1 << PB5);
}

void pwm_init()
{
    /** 8 bit PWM Initialisation
     * PWM Capable Pins: PB1, PB2, PB3, PD3, PD5, PD6.
     * Fast PWM used for easier control of the duty cycle.
     *
     * f_pwm = f_cpu / (prescaler * 256)
     * For f_pwm = 1kHz and f_cpu = 16e6, prescaler = 62.5 ~ 64
     */

    // Timer 0 - OC0A (PD6) & OC0B (PD5)
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B |= (0 << CS02) | (1 << CS01) | (1 << CS00);
    // TIMSK0 |= (1 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0);

    // Timer 1 - OC1A (PB1) & OC1B (PB2) - 8 bit used for simplicity but is capable of 10 bit pwm.
    TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
    TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10) | (1 << WGM12);
    // TIMSK1 |= (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1);

    // Timer 2 - OC2A (PB3) & OC2B (PD3)
    TCCR2A |= (1 << COM2A1) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
    TCCR2B |= (1 << CS22) | (0 << CS21) | (0 << CS20);
    // TIMSK2 |= (1 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0);
}

void pwm_start(short duty)
{
    // Turn on all pwm channels with the same duty cycle
    OCR0A = duty;
    OCR0B = duty;
    OCR1A = duty;
    OCR1B = duty;
    OCR2A = duty;
    OCR2B = duty;

    DDRB |= (1 << DDB3) | (1 << DDB2) | (1 << DDB1);
    DDRD |= (1 << DDD6) | (1 << DDD5) | (1 << DDD3);
}

void pwm_stop()
{
    // Turn off all pwm channels
    DDRB &= (0 << DDB3) | (0 << DDB2) | (0 << DDB1);
    DDRD &= (0 << DDD6) | (0 << DDD5) | (0 << DDD3);
}

void sys_init(void)
{
    // System Clocks
    clk_init();

    // UART
    usart_init();

    // Interrupts
    interrupt_init();

    // 8 bit PWM
    pwm_init();
}

int main()
{
#ifdef DEBUG
    debug_init();
#endif
    sys_init();

    usart_tx("starting...", 20);

    DDRD |= (1 << DDD4);

    while (1)
    {
        PORTD ^= (1 << PD4);
        // pwm_start(100);
        // _delay_ms(200);
        // pwm_stop();
        // _delay_ms(200);
        _delay_ms(1000);
    }

    return 0;
}
