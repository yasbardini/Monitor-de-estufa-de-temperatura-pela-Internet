#ifndef DIGITAL_H
#define DIGITAL_H

#include <stdint.h>
#include "driver/gpio.h"

#define HIGH 1
#define LOW 0

#define INPUT GPIO_MODE_INPUT
#define OUTPUT GPIO_MODE_OUTPUT

class DIGITAL
{
private:
    int debug = 0;

public:
    void pinMode(gpio_num_t gpio_num, gpio_mode_t mode);
    int digitalRead(gpio_num_t gpio_num);
    void digitalWrite(gpio_num_t gpio_num, uint32_t level);
    void setDebug(int v);
};

extern DIGITAL digital;

#endif