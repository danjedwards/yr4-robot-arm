#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "state_machine.h"
#include "nvs.h"

#include "esp_heap_caps.h"

void print_memory_usage()
{
    size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t free_internal_heap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    printf("Free heap memory: %d bytes\n", free_heap);
    printf("Free internal heap memory: %d bytes\n", free_internal_heap);
}

void app_main(void)
{
    controller_init();
    print_memory_usage();
    init_nvs();

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
