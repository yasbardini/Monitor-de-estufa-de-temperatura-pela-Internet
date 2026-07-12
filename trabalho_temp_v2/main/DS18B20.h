#ifndef DS18B20_H
#define DS18B20_H

#include "driver/gpio.h"
#include "onewire.h"
#include "digital.h"
#include "debug.h"

class DS18B20
{
private:
    ONEWIRE *onewire;
public:
    explicit DS18B20(gpio_num_t pino);

    float readTemp(void);
    void programa(void);

    void init(void);
    void init2(void);
};

#endif