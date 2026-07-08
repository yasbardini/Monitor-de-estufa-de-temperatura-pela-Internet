#include "DS18B20.h"
#include "delay.h"

#include <stdio.h>

DS18B20::DS18B20(gpio_num_t pino)
{
    DEBUG("DS18B20:Construtor\n");
    onewire = new ONEWIRE(pino);
}

void DS18B20::programa(void)
{
    onewire->reset();
    onewire->writeByte(0x4E);
    onewire->writeByte(0x00);
    onewire->writeByte(0x00);
    onewire->writeByte(0x7F);
}

void DS18B20::capturaBit(int posicao, char v[], int valor)
{
    unsigned char pbyte = posicao / 8;
    unsigned char pbit = posicao % 8;

    if (valor == 1)
        v[pbyte] |= (1 << pbit);
    else
        v[pbyte] &= ~(1 << pbit);
}

void DS18B20::fazScan(void)
{
    printf("*** fazScan***\n");

    char v[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t normal, complemento;

    onewire->reset();
    onewire->writeByte(SEARCH_ROM);

    for (int x = 0; x < 64; x++)
    {
        normal = onewire->readBit();
        complemento = onewire->readBit();

        if (normal == 0 && complemento == 0)
        {
            printf("bits conflitantes\n");
            onewire->escreve_bit(1);
            capturaBit(x, v, 1);
        }
        else if (normal == 0 && complemento == 1)
        {
            printf("b(%d)=0\n", x);
            capturaBit(x, v, 0);
            onewire->escreve_bit(0);
        }
        else if (normal == 1 && complemento == 0)
        {
            printf("b(%d)=1\n", x);
            capturaBit(x, v, 1);
            onewire->escreve_bit(1);
        }
        else
        {
            printf("Nao existem escravos no barramento\n");
            printf("Posicao do bit %d\n", x);
            return;
        }
    }

    printf("Codigo da Familia: %x\n", (unsigned char)v[0]);
    printf("Numero de Serie : %x %x %x %x %x %x\n",
           (unsigned char)v[6], (unsigned char)v[5], (unsigned char)v[4],
           (unsigned char)v[3], (unsigned char)v[2], (unsigned char)v[1]);
    printf("CRC= : %x\n", (unsigned char)v[7]);
    printf("Endereco completo: %x %x %x %x %x %x %x %x\n",
           (unsigned char)v[0], (unsigned char)v[1], (unsigned char)v[2], (unsigned char)v[3],
           (unsigned char)v[4], (unsigned char)v[5], (unsigned char)v[6], (unsigned char)v[7]);
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

char DS18B20::CRC(char end[])
{
    return 0;
}

float DS18B20::readTargetTemp(char vetor_64bits[])
{
    float temperatura;
    uint8_t a, b, inteira;
    float frac;

    onewire->reset();
    onewire->writeByte(MATCH_ROM);
    for (int x = 7; x >= 0; x--)
    {
        onewire->writeByte(vetor_64bits[x]);
    }

    onewire->writeByte(INICIA_CONVERSAO_TEMP);
    delay_ms(1000);

    onewire->reset();
    onewire->writeByte(MATCH_ROM);
    for (int x = 7; x >= 0; x--)
    {
        onewire->writeByte(vetor_64bits[x]);
    }

    onewire->writeByte(READ_TEMP_MEMORY);
    a = onewire->readByte();
    b = onewire->readByte();

    inteira = (b << 4) | (a >> 4);

    frac = ((a & 1) * 0.0625f) + (((a >> 1) & 1) * 0.125f) + (((a >> 2) & 1) * 0.25f) + (((a >> 3) & 1) * 0.5f);

    temperatura = inteira + frac;
    return temperatura;
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

    inteira = (b << 4) | (a >> 4);

    frac = ((a & 1) * 0.0625f) + (((a >> 1) & 1) * 0.125f) + (((a >> 2) & 1) * 0.25f) + (((a >> 3) & 1) * 0.5f);

    temperatura = inteira + frac;
    return temperatura;
}