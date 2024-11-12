#ifndef MOTOR
#define MOTOR

#include "driver/ledc.h"

#define MOTOR_TIMER LEDC_TIMER_0
#define MOTOR_MODE LEDC_LOW_SPEED_MODE   // The only option for esp32s3 is low speed mode
#define MOTOR_DUTY_RES LEDC_TIMER_14_BIT // Set duty resolution
#define MOTOR_DEFAULT_DUTY (4096)        // Default duty cycle. (2 ** 14) * X%
#define MOTOR_FREQUENCY (50)             // Frequency in Hertz
#define MOTOR_DUTY_RESOLUTION 1 / (MOTOR_FREQUENCY * 16384)
#define MOTOR_DUTY_MIN_US 500
#define MOTOR_DUTY_MAX_US 2500
#define MOTOR_DUTY_MIN 410  // 16384 * (MOTOR_DUTY_MIN_US / 20000)
#define MOTOR_DUTY_MAX 2048 // 16384 * (MOTOR_DUTY_MAX_US / 20000)
#define MOTOR_ANGLE_MIN -90
#define MOTOR_ANGLE_MAX 90

#define MOTOR_BASE_CHANNEL LEDC_CHANNEL_0
#define MOTOR_SHOULDER_CHANNEL LEDC_CHANNEL_1
#define MOTOR_ELBOW_CHANNEL LEDC_CHANNEL_2
#define MOTOR_WRIST_1_CHANNEL LEDC_CHANNEL_3
#define MOTOR_WRIST_2_CHANNEL LEDC_CHANNEL_4
#define MOTOR_GRIPPER_CHANNEL LEDC_CHANNEL_5

void motor_init(uint8_t channel, uint8_t gpio);
void motor_to_angle(uint8_t channel, int angle);
int motor_angle_to_duty(int angle);

#endif // MOTOR