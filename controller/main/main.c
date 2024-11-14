#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "state_machine.h"
#include "nvs.h"

void app_main(void)
{
    controller_init();

    current_state->init();

    while (1)
    {
        if (next_state != NULL)
        {
            current_state = next_state;
            next_state = NULL;
            current_state->init();
        }

        current_state->process();
    }
}
