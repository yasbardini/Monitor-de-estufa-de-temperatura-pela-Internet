#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <stdint.h>
#include "driver/gpio.h"
#include "Digital.h"

class OneWire
{
public:
    explicit OneWire(gpio_num_t pin);

    bool reset();
    void writeBit(uint8_t bit);
    uint8_t readBit();
    void writeByte(uint8_t value);
    uint8_t readByte();

private:
    gpio_num_t pin;
    Digital digital;

    void driveLow();
    void releaseBus();
};

#endif