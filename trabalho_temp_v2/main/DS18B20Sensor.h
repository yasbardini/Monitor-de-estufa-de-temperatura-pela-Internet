#ifndef DS18B20_SENSOR_H
#define DS18B20_SENSOR_H

#include "driver/gpio.h"
#include "OneWire.h"

class DS18B20Sensor
{
public:
    explicit DS18B20Sensor(gpio_num_t pin);

    bool begin();
    float read();

private:
    OneWire oneWire;
};

#endif