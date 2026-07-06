#include "OneWire.h"
#include "esp_rom_sys.h"

OneWire::OneWire(gpio_num_t pin) : pin(pin)
{
    releaseBus();
}

void OneWire::driveLow()
{
    digital.pinMode(pin, GPIO_MODE_OUTPUT);
    digital.write(pin, 0);
}

void OneWire::releaseBus()
{
    digital.pinMode(pin, GPIO_MODE_INPUT);
}

bool OneWire::reset()
{
    driveLow();
    esp_rom_delay_us(480);
    releaseBus();
    esp_rom_delay_us(70);

    bool presence = (digital.read(pin) == 0);

    esp_rom_delay_us(410);
    return presence;
}

void OneWire::writeBit(uint8_t bit)
{
    driveLow();

    if (bit)
    {
        esp_rom_delay_us(6);
        releaseBus();
        esp_rom_delay_us(64);
    }
    else
    {
        esp_rom_delay_us(60);
        releaseBus();
        esp_rom_delay_us(10);
    }
}

uint8_t OneWire::readBit()
{
    driveLow();
    esp_rom_delay_us(6);
    releaseBus();
    esp_rom_delay_us(9);

    uint8_t bit = digital.read(pin);

    esp_rom_delay_us(55);
    return bit;
}

void OneWire::writeByte(uint8_t value)
{
    for (int i = 0; i < 8; i++)
    {
        writeBit((value >> i) & 0x01);
    }
}

uint8_t OneWire::readByte()
{
    uint8_t value = 0;
    for (int i = 0; i < 8; i++)
    {
        value |= (readBit() << i);
    }
    return value;
}