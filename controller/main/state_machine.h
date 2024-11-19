#ifndef STATE_MACHINE
#define STATE_MACHINE

#include "freertos/semphr.h"
#include <stdio.h>
#include "server.h"
#include "nvs.h"

typedef void (*state_function_t)();

typedef struct
{
    uint8_t id;
    char *name;
    state_function_t init;
    state_function_t process;
} state;

typedef struct
{
    state *current_state;
    state *next_state;
    uint8_t current_waypoint_index;
    waypoint current_program[MAX_WAYPOINTS];
} state_machine;

void state_machine_init();

extern state_machine sm;
extern SemaphoreHandle_t sm_mutex;

#endif // STATE_MACHINE