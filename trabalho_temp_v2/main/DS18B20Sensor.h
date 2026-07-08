#ifndef DS18B20_SENSOR_WRAPPER_H
#define DS18B20_SENSOR_WRAPPER_H

#include <stdbool.h>
#include "driver/gpio.h"
#include "DS18B20.h"

class DS18B20Sensor
{
public:
    explicit DS18B20Sensor(gpio_num_t pin);

    void begin();
    float read();

private:
    gpio_num_t pin;
    DS18B20 sensor;
    bool initialized;
};

#endif