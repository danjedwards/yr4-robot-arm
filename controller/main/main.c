#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "server.h"
#include "motor.h"
#include "string.h"

void proc_client_message(const char *data, int len)
{
    char cmd = data[0];

    int base_duty = ((data[1] & 0x0F) << 8) | data[2];
    printf("base position %i\n", base_duty);
    motor_to_pos(MOTOR_BASE_CHANNEL, base_duty);

    int shoulder_duty = ((data[3] & 0x0F) << 8) | data[4];
    printf("shoulder position %i\n", shoulder_duty);
    motor_to_pos(MOTOR_SHOULDER_CHANNEL, shoulder_duty);

    int elbow_duty = ((data[5] & 0x0F) << 8) | data[6];
    printf("elbow position %i\n", elbow_duty);
    motor_to_pos(MOTOR_ELBOW_CHANNEL, elbow_duty);

    int wrist1_duty = ((data[7] & 0x0F) << 8) | data[8];
    printf("wrist1 position %i\n", wrist1_duty);
    motor_to_pos(MOTOR_WRIST_1_CHANNEL, wrist1_duty);

    int wrist2_duty = ((data[9] & 0x0F) << 8) | data[10];
    printf("wrist2 position %i\n", wrist2_duty);
    motor_to_pos(MOTOR_WRIST_2_CHANNEL, wrist2_duty);

    int gripper_duty = ((data[11] & 0x0F) << 8) | data[12];
    printf("gripper position %i\n", gripper_duty);
    motor_to_pos(MOTOR_GRIPPER_CHANNEL, gripper_duty);
}

void controller_init()
{
    // Initialise the tcp server
    server_init();

    register_message_callback(proc_client_message);

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
    int duty = MOTOR_DUTY_MIN + 1, step = 1;
    while (1)
    {
        motor_to_pos(MOTOR_BASE_CHANNEL, duty);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        if (duty == MOTOR_DUTY_MAX || duty == MOTOR_DUTY_MIN)
        {
            step = -step;
        }
        duty += step;
    }
}
