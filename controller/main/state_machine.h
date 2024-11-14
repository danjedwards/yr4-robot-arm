#ifndef STATE_MACHINE
#define STATE_MACHINE

#include <stdio.h>
#include "server.h"

#define IDLE 0
#define PROGRAM 1
#define RUN 2

typedef void (*state_function_t)();

typedef struct
{
    uint8_t id;
    char *name;
    state_function_t init;
    state_function_t process;
} state;

void controller_init();

void idle_init();
void idle_proc();

void prog_init();
void prog_proc();

extern state idle;
extern state program;
extern volatile state *current_state;
extern volatile state *next_state;

#endif // STATE_MACHINE