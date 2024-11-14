#ifndef MOTOR
#define MOTOR

#include <stdio.h>

#define MOTOR_TIMER LEDC_TIMER_0
#define MOTOR_MODE LEDC_LOW_SPEED_MODE   // The only option for esp32s3 is low speed mode
#define MOTOR_DUTY_RES LEDC_TIMER_14_BIT // Set duty resolution
#define MOTOR_DEFAULT_DUTY (1000)        // Default duty cycle. (2 ** 14) * X%
#define MOTOR_FREQUENCY (50)             // Frequency in Hertz
#define MOTOR_DUTY_MIN 410               // 16384 * (MOTOR_DUTY_MIN_US / 20000)
#define MOTOR_DUTY_MAX 2048              // 16384 * (MOTOR_DUTY_MAX_US / 20000)

void motor_init(uint8_t channel, uint8_t gpio);
void motor_to_pos(uint8_t motor_channel, int duty);

#endif // MOTOR