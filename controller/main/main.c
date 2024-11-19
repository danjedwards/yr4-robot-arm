#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "state_machine.h"

void app_main(void)
{
    state_machine_init();

    while (1)
    {
        if (xSemaphoreTake(sm_mutex, portMAX_DELAY) != pdTRUE)
        {
            printf("State machine mutex error in main!\n");
        }

        if (sm.next_state != NULL)
        {
            sm.current_state = sm.next_state;
            sm.next_state = NULL;
            sm.current_state->init();
        }

        sm.current_state->process();
        xSemaphoreGive(sm_mutex);
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}
