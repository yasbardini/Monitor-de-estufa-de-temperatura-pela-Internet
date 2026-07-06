#ifndef DIGITAL_H
#define DIGITAL_H

#include "driver/gpio.h"

class Digital
{
public:
    void pinMode(gpio_num_t pin, gpio_mode_t mode);
    void write(gpio_num_t pin, uint32_t level);
    int read(gpio_num_t pin);
};

#endif