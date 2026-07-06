#include "DS18B20Sensor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static constexpr uint8_t DS18B20_CMD_SKIP_ROM = 0xCC;
static constexpr uint8_t DS18B20_CMD_CONVERT_T = 0x44;
static constexpr uint8_t DS18B20_CMD_READ_SCRATCHPAD = 0xBE;

DS18B20Sensor::DS18B20Sensor(gpio_num_t pin) : oneWire(pin) {}

bool DS18B20Sensor::begin()
{
    return oneWire.reset();
}

float DS18B20Sensor::read()
{
    if (!oneWire.reset())
    {
        return -127.0f;
    }

    oneWire.writeByte(DS18B20_CMD_SKIP_ROM);
    oneWire.writeByte(DS18B20_CMD_CONVERT_T);

    vTaskDelay(pdMS_TO_TICKS(800));

    if (!oneWire.reset())
    {
        return -127.0f;
    }

    oneWire.writeByte(DS18B20_CMD_SKIP_ROM);
    oneWire.writeByte(DS18B20_CMD_READ_SCRATCHPAD);

    uint8_t tempLSB = oneWire.readByte();
    uint8_t tempMSB = oneWire.readByte();

    int16_t raw = (tempMSB << 8) | tempLSB;
    return raw / 16.0f;
}