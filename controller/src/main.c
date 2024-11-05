#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

void writeGRB(int pin, int GRB)
{
    int mask = 0x800000;
    volatile int duty = 0;
    for (int j = 0; j < 24; j++)
    {
        gpio_set_level(pin, 1);
        if ((GRB & mask))
        {
            for (volatile int i = 0; i < 3; i++)
            {
            }
            duty = 0;
            duty = 0;
            gpio_set_level(pin, 0);
            duty++;
            duty = 0;
            duty = 0;
            duty = 0;
        }
        else
        {
            duty = 0;
            gpio_set_level(pin, 0);
            for (volatile int i = 0; i < 5; i++)
            {
            }
        }
        mask = mask >> 1;
    }
    vTaskDelay(60 / portTICK_PERIOD_MS);
}

void app_main(void)
{
    int pin = 48;
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    int color = 0x0000FF;
    writeGRB(pin, color);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    color = 0x00FF00;
    writeGRB(pin, color);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    color = 0xFF0000;
    writeGRB(pin, color);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}