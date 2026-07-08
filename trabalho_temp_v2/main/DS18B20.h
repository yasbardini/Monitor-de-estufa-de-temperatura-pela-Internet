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

    char CRC(char end[]);
    void capturaBit(int posicao, char v[], int valor);

public:
    explicit DS18B20(gpio_num_t pino);

    float readTemp(void);
    void programa(void);
    float readTargetTemp(char end[]);

    void init(void);
    void init2(void);
    void fazScan(void);
};

#endif