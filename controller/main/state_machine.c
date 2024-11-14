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

state idle = {MSG_IDLE, "idle", idle_init, idle_proc};
state program = {MSG_PROGRAM, "program", prog_init, prog_proc};

volatile state *current_state = &idle;
volatile state *next_state = NULL;

///////////////////////////////////////////////////////////////////////
// Common
///////////////////////////////////////////////////////////////////////

void controller_init()
{
    init_nvs();
    server_init();

    // Motors
    motor_init(BASE_PWM_CHANNEL, BASE_GPIO);
    motor_init(SHOULDER_PWM_CHANNEL, SHOULDER_GPIO);
    motor_init(ELBOW_PWM_CHANNEL, ELBOW_GPIO);
    motor_init(WRIST_1_PWM_CHANNEL, WRIST_1_GPIO);
    motor_init(WRIST_2_PWM_CHANNEL, WRIST_2_GPIO);
    motor_init(GRIPPER_PWM_CHANNEL, GRIPPER_GPIO);
}

// This is NOT thread safe!!!!!!!!!!!!!!!!!!
void process_message_callback(const message request, message *response)
{
    response->rw = request.rw;
    response->state = request.state;
    response->command = request.command;
    response->data_len = 0;

    // Could do with better error checking.

    switch (response->command)
    {
    case MSG_WAYPOINT_COUNT:
        response->data_len = 1;
        response->data[0] = MAX_WAYPOINTS;
        break;
    case MSG_WAYPOINT_IDX:
        if (response->rw == MSG_WRITE && request.data[0] > 0 && request.data[0] < MAX_WAYPOINTS)
        {
            current_waypoint = request.data[0];
        }
        response->data_len = 1;
        response->data[0] = current_waypoint;
        break;
    case MSG_WAYPOINT_CUR:
        if (response->rw == MSG_WRITE)
        {
            memcpy(&current_program[current_waypoint], request.data, request.data_len);
        }
        response->data_len = sizeof(waypoint);
        memcpy(response->data, &current_program[current_waypoint], response->data_len);
        break;
    case MSG_PROGRAM_COUNT:
        response->data_len = 1;
        response->data[0] = MAX_PROGRAMS;
        break;
    case MSG_PROGRAM_NAME:
        if (response->rw == MSG_WRITE)
        {
            set_program_name(request.data[0], request.data + 1);
        }
        char *name = get_program_name(request.data[0]);
        response->data_len = strlen(name);
        memcpy(response->data, name, response->data_len);
        break;
    case MSG_PROGRAM_RW:
        if (response->rw == MSG_WRITE)
        {
            save_program(request.data[0]);
        }
        else
        {
            load_program(request.data[0]);
        }
        char msg[30];
        snprintf(msg, 30, "Loaded program: %x", request.data[0]);
        response->data_len = strlen(msg);
        memcpy(response->data, msg, response->data_len);
        break;
    default:
        return;
    }

    response->err = MSG_OKAY;
}

///////////////////////////////////////////////////////////////////////
// Idle
///////////////////////////////////////////////////////////////////////

void idle_init()
{
    register_message_callback(process_message_callback);
}

void idle_proc()
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("idle...\n");
}

///////////////////////////////////////////////////////////////////////
// Program
///////////////////////////////////////////////////////////////////////

void prog_init()
{
    register_message_callback(process_message_callback);
}

void prog_proc()
{
}