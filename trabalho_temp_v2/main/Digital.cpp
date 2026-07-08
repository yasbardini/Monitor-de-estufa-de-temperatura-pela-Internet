#include "digital.h"

#include <stdio.h>

void DIGITAL::pinMode(gpio_num_t gpio_num, gpio_mode_t mode)
{
    switch (mode)
    {
    case GPIO_PULLUP_ONLY:
        if (debug)
            printf("DEBUG::configura GPIO_PULLUP_ONLY\n");
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
        gpio_set_pull_mode(gpio_num, GPIO_PULLUP_ONLY);
        break;

    case GPIO_MODE_INPUT:
        if (debug)
            printf("DEBUG::configura GPIO_MODE_INPUT\n");
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
        gpio_set_pull_mode(gpio_num, GPIO_FLOATING);
        break;

    case GPIO_MODE_OUTPUT:
        if (debug)
            printf("DEBUG::configura GPIO_MODE_OUTPUT\n");
        gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
        break;

    default:
        break;
    }
}

int DIGITAL::digitalRead(gpio_num_t gpio_num)
{
    int v = gpio_get_level(gpio_num);

    if (debug)
        printf("DEBUG::digitalRead %d %s\n", (int)gpio_num, (v == 1) ? "HIGH" : "LOW");

    return v;
}

void DIGITAL::setDebug(int v)
{
    debug = v;
}

void DIGITAL::digitalWrite(gpio_num_t gpio_num, uint32_t level)
{
    if (debug)
        printf("DEBUG::digitalWrite %d %s\n", (int)gpio_num, (level == 1) ? "HIGH" : "LOW");

    gpio_set_level(gpio_num, level);
}

DIGITAL digital = DIGITAL();