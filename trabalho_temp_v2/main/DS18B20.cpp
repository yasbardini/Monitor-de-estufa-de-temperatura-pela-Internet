#include "DS18B20.h"
#include "delay.h"

#include <stdio.h>

DS18B20::DS18B20(gpio_num_t pino)
{
    DEBUG("DS18B20:Construtor\n");
    onewire = new ONEWIRE(pino);
}

void DS18B20::programa(void) // função que programa o sensor, configurando a resolução e o modo de operação
{
    onewire->reset();
    onewire->writeByte(0x4E);
    onewire->writeByte(0x00);
    onewire->writeByte(0x00);
    onewire->writeByte(0x7F);
}

void DS18B20::init(void)
{
    uint8_t serial[6], crc;
    printf("*** init***\n");
    delay_ms(500);
    printf("INIT\n");
    if (onewire->reset() == 0)
        printf("Detectou escravo na linha\n");
    else
        printf("Nao detectou escravo\n");

    onewire->writeByte(READ_ROM);

    printf("Codigo da Familia: %d\n", onewire->readByte());
    for (uint8_t x = 0; x < 6; x++)
        serial[x] = onewire->readByte();

    printf("Numero de Serie : %d %d %d %d %d %d\n",
           serial[0], serial[1], serial[2], serial[3], serial[4], serial[5]);

    crc = onewire->readByte();
    printf("CRC= : %d\n", crc);

    delay_ms(1000);
}

void DS18B20::init2(void)
{
    printf("*** init2***\n");
    delay_ms(500);

    printf("INIT\n");
    if (onewire->reset() == 0)
        printf("Detectou escravo na linha\n");
    else
        printf("Nao detectou escravo\n");

    delay_ms(1000);
}

float DS18B20::readTemp(void)
{
    float temperatura;
    uint8_t a, b, inteira;
    float frac;

    onewire->reset();
    onewire->writeByte(SKIP_ROM);
    onewire->writeByte(INICIA_CONVERSAO_TEMP);
    delay_ms(1000);

    onewire->reset();
    onewire->writeByte(SKIP_ROM);
    onewire->writeByte(READ_TEMP_MEMORY);

    a = onewire->readByte();
    b = onewire->readByte();
    printf("a=%u b=%u\n", a, b);

    inteira = (b << 4) | (a >> 4);

    frac = ((a & 1) * 0.0625f) + (((a >> 1) & 1) * 0.125f) + (((a >> 2) & 1) * 0.25f) + (((a >> 3) & 1) * 0.5f);

    temperatura = inteira + frac;
    return temperatura;
}