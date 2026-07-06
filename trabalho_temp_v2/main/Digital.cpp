#include "Digital.h"

void Digital::pinMode(gpio_num_t pin, gpio_mode_t mode)
{
    gpio_set_direction(pin, mode);
}

void Digital::write(gpio_num_t pin, uint32_t level)
{
    gpio_set_level(pin, level);
}

int Digital::read(gpio_num_t pin)
{
    return gpio_get_level(pin);
}