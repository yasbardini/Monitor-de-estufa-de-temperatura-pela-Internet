#ifndef __ONE_WIRE__
#define __ONE_WIRE__

#include <stdint.h>
#include "driver/gpio.h"
#include "digital.h"
#include "debug.h"

#define INICIA_CONVERSAO_TEMP 0x44
#define READ_TEMP_MEMORY 0xBE
#define SKIP_ROM 0xCC
#define READ_ROM 0x33
#define MATCH_ROM 0x55
#define SEARCH_ROM 0xF0

class ONEWIRE
{
private:
    uint8_t tmp;
    gpio_num_t PIN_DADOS;

    void low(void);
    void high(void);

public:
    explicit ONEWIRE(gpio_num_t p);

    void config(gpio_num_t pino);
    uint8_t reset(void);

    void escreve_bit(uint8_t bit);
    void writeByte(uint8_t v);

    uint8_t readBit(void);
    uint8_t readByte(void);
};

#endif