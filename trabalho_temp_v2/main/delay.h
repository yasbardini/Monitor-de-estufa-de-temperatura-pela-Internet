#ifndef DELAY_H
#define DELAY_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"

static inline void delay_ms(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

static inline void delay_us(uint32_t us)
{
    esp_rom_delay_us(us);
}

#endif