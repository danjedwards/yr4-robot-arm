#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "server.h"

void controller_init()
{
    // Initialise the tcp server
    server_init();

    //
}

void app_main(void)
{
    controller_init();
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
