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

void idle_init();
void idle_proc();
void run_init();
void run_proc();

state program = {MSG_IDLE, "program", idle_init, idle_proc};
state run = {MSG_RUNNING, "run", run_init, run_proc};

state_machine sm = {0};
SemaphoreHandle_t sm_mutex = {0};

///////////////////////////////////////////////////////////////////////
// Common
///////////////////////////////////////////////////////////////////////

void state_machine_init()
{

    sm_mutex = xSemaphoreCreateMutex();
    if (sm_mutex == NULL)
    {
        printf("Failed to create mutex\n");
        return;
    }

    sm.current_state = &program;
    sm.next_state = NULL;
    sm.current_waypoint_index = 0;

    for (int i = 0; i < MAX_WAYPOINTS; i++)
    {
        sm.current_program[i].base_pos = MOTOR_DEFAULT_DUTY;
        sm.current_program[i].shoulder_pos = MOTOR_DEFAULT_DUTY;
        sm.current_program[i].elbow_pos = MOTOR_DEFAULT_DUTY;
        sm.current_program[i].wrist1_pos = MOTOR_DEFAULT_DUTY;
        sm.current_program[i].wrist1_pos = MOTOR_DEFAULT_DUTY;
        sm.current_program[i].gripper_pos = MOTOR_DEFAULT_DUTY;
    }

    // memset(sm.current_program, 410, sizeof(sm.current_program));
    
    init_nvs();
    server_init();

    sm.current_state->init();
}

void process_message_callback(const message request, message *response)
{
    response->rw = request.rw;
    response->state = request.state;
    response->command = request.command;
    response->data_len = 0;

    if (xSemaphoreTake(sm_mutex, portMAX_DELAY) != pdTRUE)
    {
        printf("State machine mutex error!\n");
        return;
    }


    // If state == running then dont allow modification of position!
    switch (response->command)
    {
    case MSG_WAYPOINT_COUNT:
        response->data_len = 1;
        response->data[0] = MAX_WAYPOINTS;
        break;
    case MSG_WAYPOINT_IDX:
        if (response->rw == MSG_WRITE)
        {
            if (request.data_len == 0)
                goto end;
            if (request.data[0] > MAX_WAYPOINTS - 1)
                goto end;
            sm.current_waypoint_index = request.data[0];
        }
        response->data_len = 1;
        response->data[0] = sm.current_waypoint_index;
        break;
    case MSG_WAYPOINT_CUR:
        if (response->rw == MSG_WRITE)
        {
            if (request.data_len != 12)
                goto end;
            for (int i = 0; i < request.data_len / 2; i++)
            {
                uint16_t data = (request.data[2 * i + 1] << 8) | request.data[2 * i];
                if (data < MOTOR_DUTY_MIN || data > MOTOR_DUTY_MAX)
                {
                    goto end;
                }
            }
            memcpy(&sm.current_program[sm.current_waypoint_index], request.data, request.data_len);
        }
        response->data_len = sizeof(waypoint);
        memcpy(response->data, &sm.current_program[sm.current_waypoint_index], response->data_len);
        // debug
        printf("base pos %d\n", sm.current_program[sm.current_waypoint_index].base_pos);
        printf("shoulder pos %d\n", sm.current_program[sm.current_waypoint_index].shoulder_pos);
        printf("elbow pos %d\n", sm.current_program[sm.current_waypoint_index].elbow_pos);
        printf("wrist1 pos %d\n", sm.current_program[sm.current_waypoint_index].wrist1_pos);
        printf("wrist2 pos %d\n", sm.current_program[sm.current_waypoint_index].wrist2_pos);
        printf("gripper pos %d\n", sm.current_program[sm.current_waypoint_index].gripper_pos);
        break;
    case MSG_PROGRAM_COUNT:
        response->data_len = 1;
        response->data[0] = MAX_PROGRAMS;
        break;
    case MSG_PROGRAM_NAME:
        if (request.data_len == 0)
            goto end;
        if (request.data[0] > MAX_PROGRAMS)
            goto end;
        if (response->rw == MSG_WRITE)
        {
            if (request.data_len < 2)
                goto end;
            char new_name[MAX_NAME_LEN];
            memcpy(new_name, request.data + 1, request.data_len - 1);
            new_name[request.data_len - 1] = '\0';
            set_program_name(request.data[0], new_name);
        }
        char *name = get_program_name(request.data[0]);
        if (name == NULL)
            goto end;
        response->data_len = strlen(name) + 1;
        response->data[0] = request.data[0];
        memcpy(response->data + 1, name, response->data_len);
        break;
    case MSG_PROGRAM_RW:
        if (request.data_len == 0)
            goto end;
        if (request.data[0] > MAX_PROGRAMS - 1)
            goto end;
        if (response->rw == MSG_WRITE)
        {
            save_program(request.data[0], sm.current_program);
        }
        else
        {
            load_program(request.data[0], sm.current_program);
        }
        char msg[30];
        snprintf(msg, 30, "Program %x Loaded", request.data[0]);
        response->data_len = strlen(msg);
        memcpy(response->data, msg, response->data_len);
        break;
    case MSG_RUN:
        if (response->rw == MSG_WRITE)
        {
            if (request.data_len == 0)
                goto end;
            sm.next_state = request.data[0] ? &run : &program;
        }
        break;
    default:
        printf("Unkown Command!\n");
        goto end;
    }

    if (sm.next_state != NULL)
    {
        response->state = sm.next_state->id;
    }
    else
    {
        response->state = sm.current_state->id;
    }

    response->err = MSG_OKAY;
end:
    xSemaphoreGive(sm_mutex);
    return;
}

///////////////////////////////////////////////////////////////////////
// Idle
///////////////////////////////////////////////////////////////////////

void idle_init()
{
    printf("Idle State...\n");
    register_message_callback(process_message_callback);

    motor_init(BASE_PWM_CHANNEL, BASE_GPIO);
    motor_init(SHOULDER_PWM_CHANNEL, SHOULDER_GPIO);
    motor_init(ELBOW_PWM_CHANNEL, ELBOW_GPIO);
    motor_init(WRIST_1_PWM_CHANNEL, WRIST_1_GPIO);
    motor_init(WRIST_2_PWM_CHANNEL, WRIST_2_GPIO);
    motor_init(GRIPPER_PWM_CHANNEL, GRIPPER_GPIO);
}

void idle_proc()
{
    vTaskDelay(500 / portTICK_PERIOD_MS);

    waypoint wp = sm.current_program[sm.current_waypoint_index];

    motor_to_pos(BASE_PWM_CHANNEL, wp.base_pos);
    motor_to_pos(SHOULDER_PWM_CHANNEL, wp.shoulder_pos);
    motor_to_pos(ELBOW_PWM_CHANNEL, wp.elbow_pos);
    motor_to_pos(WRIST_1_PWM_CHANNEL, wp.wrist1_pos);
    motor_to_pos(WRIST_2_PWM_CHANNEL, wp.wrist2_pos);
    motor_to_pos(GRIPPER_PWM_CHANNEL, wp.gripper_pos);

    printf("gripper pos %d\n", sm.current_program[sm.current_waypoint_index].gripper_pos);

}

///////////////////////////////////////////////////////////////////////
// Run
///////////////////////////////////////////////////////////////////////

static int memvcmp(void *memory, unsigned char val, unsigned int size)
{
    unsigned char *mm = (unsigned char*)memory;
    return (*mm == val) && memcmp(mm, mm + 1, size - 1) == 0;
}

void run_init()
{
    printf("Running...\n");
    register_message_callback(process_message_callback);
}

void run_proc()
{
    // Program finished
    if (sm.current_waypoint_index > MAX_WAYPOINTS)
    {
        printf("Program Finished!\n");
        sm.current_waypoint_index = 0;
        sm.next_state = &program;
        return;
    }

    // Voided waypoint
    if (memvcmp(&sm.current_program[sm.current_waypoint_index], 0, sizeof(waypoint)))
    {
        printf("Waypoint index %d is a voided waypoint! Skipping...\n", sm.current_waypoint_index);
        goto end;
    }

    // Move closer to the target position
    waypoint wp = sm.current_program[sm.current_waypoint_index];
    
    motor_to_pos(BASE_PWM_CHANNEL, wp.base_pos);
    motor_to_pos(SHOULDER_PWM_CHANNEL, wp.shoulder_pos);
    motor_to_pos(ELBOW_PWM_CHANNEL, wp.elbow_pos);
    motor_to_pos(WRIST_1_PWM_CHANNEL, wp.wrist1_pos);
    motor_to_pos(WRIST_2_PWM_CHANNEL, wp.wrist2_pos);
    motor_to_pos(GRIPPER_PWM_CHANNEL, wp.gripper_pos);

    vTaskDelay(500 / portTICK_PERIOD_MS);

end:
    sm.current_waypoint_index++;
}