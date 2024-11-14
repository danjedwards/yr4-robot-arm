#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "state_machine.h"
#include "motor.h"
#include "server.h"
#include "io.h"
#include "nvs.h"

///////////////////////////////////////////////////////////////////////
// States
///////////////////////////////////////////////////////////////////////

state idle = {IDLE, "idle", idle_init, idle_proc};
state program = {PROGRAM, "program", prog_init, prog_proc};

volatile state *current_state = &idle;
volatile state *next_state = NULL;

///////////////////////////////////////////////////////////////////////
// Common
///////////////////////////////////////////////////////////////////////

void controller_init()
{
    // Initialise the tcp server
    server_init();

    // Initialise Motors
    motor_init(BASE_PWM_CHANNEL, BASE_GPIO);
    motor_init(SHOULDER_PWM_CHANNEL, SHOULDER_GPIO);
    motor_init(ELBOW_PWM_CHANNEL, ELBOW_GPIO);
    motor_init(WRIST_1_PWM_CHANNEL, WRIST_1_GPIO);
    motor_init(WRIST_2_PWM_CHANNEL, WRIST_2_GPIO);
    motor_init(GRIPPER_PWM_CHANNEL, GRIPPER_GPIO);
}

///////////////////////////////////////////////////////////////////////
// Idle
///////////////////////////////////////////////////////////////////////

void idle_msg_callback(const message request, message *response)
{
    printf("Idle received message...\n");
    response->err = MSG_OKAY;
    response->rw = request.rw;
    response->mode = request.mode;
    response->command = request.command;
    strcpy(response->data, "ugly ass shit\n\0");
    response->data_len = 15;
}

void idle_init()
{
    register_message_callback(idle_msg_callback);
}

void idle_proc()
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("idle...\n");
}

///////////////////////////////////////////////////////////////////////
// Program
///////////////////////////////////////////////////////////////////////

void prog_msg_callback(const message request, message *response)
{
    printf("Prog received message...\n");
    response->err = MSG_OKAY;
}

void prog_init()
{
    register_message_callback(prog_msg_callback);
}

void prog_proc()
{
}