#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "server.h"
#include "motor.h"

void controller_init()
{
    // Initialise the tcp server
    server_init();

    // Initialise Motors
    motor_init(MOTOR_BASE_CHANNEL, 5);
    motor_init(MOTOR_SHOULDER_CHANNEL, 1);
    motor_init(MOTOR_ELBOW_CHANNEL, 2);
    motor_init(MOTOR_WRIST_1_CHANNEL, 3);
    motor_init(MOTOR_WRIST_2_CHANNEL, 4);
    motor_init(MOTOR_GRIPPER_CHANNEL, 0);
}

void app_main(void)
{
    controller_init();
    while (1)
    {
        motor_to_angle(MOTOR_BASE_CHANNEL, -90);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        motor_to_angle(MOTOR_BASE_CHANNEL, -45);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        motor_to_angle(MOTOR_BASE_CHANNEL, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        motor_to_angle(MOTOR_BASE_CHANNEL, 45);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        motor_to_angle(MOTOR_BASE_CHANNEL, 90);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        motor_to_angle(MOTOR_BASE_CHANNEL, 45);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        motor_to_angle(MOTOR_BASE_CHANNEL, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        motor_to_angle(MOTOR_BASE_CHANNEL, -45);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
