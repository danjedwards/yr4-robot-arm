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
    return "serial disabled in debug mode";
#endif

    unsigned char dummy;
    while (UCSR0A & (1 << RXC0))
        dummy = UDR0;
}

void usart_tx(char *msg, int msg_len)
{
#ifdef DEBUG
    return;
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

    // Enable global interrupts
    sei();
}

ISR(INT0_vect)
{
    // TO DO
}

void io_init()
{
    /**
     ** 13.4.1 MCUCR – MCU Control Register
     * Disable internal pull ups even if DDxn and PORTxn are configured to pull up.
     *
     ** 13.4.2 PORTB – The Port B Data Register
     ** 13.4.3 DDRB – The Port B Data Direction Register
     ** 13.4.4 PINB – The Port B Input Pins Address (Read Only)
     ** 13.4.5 PORTC – The Port C Data Register
     ** 13.4.6 DDRC – The Port C Data Direction Register
     ** 13.4.7 PINC – The Port C Input Pins Address (Read Only)
     ** 13.4.8 PORTD – The Port D Data Register
     ** 13.4.9 DDRD – The Port D Data Direction Register
     ** 13.4.10 PIND – The Port D Input Pins Address (Read Only)
     */

    // When complete, state why each pin is configured as an Input/Output
    DDRB |= (1 << PB1);
    DDRD |= (1 << PD3);
}

void pwm_8bit_init()
{
    /** 8 bit PWM Initialisation
     *** PWM Capable Pins
     * PB1, PB2, PB3, PD3, PD5, PD6.
     *
     *** Assumptions
     * IO is configured correctly before this function.
     *
     *** Registers
     * 14.9.1 TCCR0A – Timer/Counter Control Register A
     * Comparison register behaviour configuration.
     *
     * 14.9.2 TCCR0B – Timer/Counter Control Register B
     * Comparison register behaviour configuration and clock configuration.
     *
     * 14.9.3 TCNT0 – Timer/Counter Register
     * Read/write. Contains timer value.
     *
     * 14.9.4 OCR0A – Output Compare Register A
     * Output compare register A value.
     *
     * 14.9.5 OCR0B – Output Compare Register B
     * Output compare register B value.
     *
     * 14.9.6 TIMSK0 – Timer/Counter Interrupt Mask Register
     * Interrupt enable/disable for output compare match.
     *
     * 14.9.7 TIFR0 – Timer/Counter 0 Interrupt Flag Register
     * Flags for match and overflow.
     */

    TCCR0A |= (1 << COM0A0); // COM0A0 = 1, COM0A1 = 0 => Toggle OC0A on compare match
    TCCR0A |= (1 << COM0B0); // COM0B0 = 1, COM0B1 = 0 => Toggle OC0B on compare match
    // TCCR0A |= (1 << WGM00) // 00 => Update OCRx immediately
}

void pwm_8bit_drive()
{
}

void pwm_16bit_init()
{
}

void pwm_16bit_drive()
{
}

void sys_init(void)
{
    // System Clocks
    clk_init();

    // UART
    usart_init();

    // Interrupts
    interrupt_init();

    // IO
    io_init();

    // 8 bit PWM
    pwm_16bit_init();

    // 16 bit PWM
}

int main()
{
#ifdef DEBUG
    debug_init();
#endif
    sys_init();

    usart_flush();
    while (1)
    {
        char msg[10];
        usart_read_string(msg, 10);
        usart_tx(msg, 10);
    }

    return 0;
}