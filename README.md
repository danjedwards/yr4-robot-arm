# Year 4 Robot Arm Code
Embedded software for controlling a customised Lynxmotion ALD5 robot arm.

# Project Structure
`src` - project source code.
`test` - project unit tests.

# To DO
- ~~Understand how to change fuse bytes,~~
- Implement USART code, => Works at a baud rate of 9600 but not at a baud rate of 15200 :( needs a clean up. 
- ~~Implement Interrupts => Half done~~, => done for PCINT23 with pull up. Tiggers on pin change.
- ~~Implement IO => Half done~~ => Initialise IO in other initialise functions. For example, in PWM init.
- ~~Implement PWM~~,
- ~~Implement ADC~~ => working on ADC0,
- ~~Implement Timers?~~ => all used for pwm.
- Implement WDT?
- Implement RW to EEPROM, => Can't test on Aruino because this requires changing the fuse bits (ISP required).
- Implement Initialise HW,
- Create high level design for the system,
- Write unit tests,
- Implement design,
- Verify/Validate everything.

# Improvement Ideas


# Reference Material
Platform IO avr template - https://gitlab.com/jjchico-edc/avr-pio-template