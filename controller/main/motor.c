#include "motor.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

void motor_init(uint8_t channel, uint8_t gpio)
{
    // Prepare and then apply the LEDC PWM motor_timer configuration
    ledc_timer_config_t motor_timer = {
        .speed_mode = MOTOR_MODE,
        .duty_resolution = MOTOR_DUTY_RES,
        .timer_num = MOTOR_TIMER,
        .freq_hz = MOTOR_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&motor_timer));

    // Prepare and then apply the LEDC PWM motor_channel configuration
    ledc_channel_config_t motor_channel = {
        .speed_mode = MOTOR_MODE,
        .channel = channel,
        .timer_sel = MOTOR_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = gpio,
        .duty = MOTOR_DEFAULT_DUTY,
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&motor_channel));
}

void motor_to_pos(uint8_t motor_channel, int duty)
{
    if (duty < MOTOR_DUTY_MIN || duty > MOTOR_DUTY_MAX)
    {
        return;
    }
    ledc_set_duty(MOTOR_MODE, motor_channel, duty);
    ledc_update_duty(MOTOR_MODE, motor_channel);
}