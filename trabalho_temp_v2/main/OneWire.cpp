#include "onewire.h"
#include "delay.h"

ONEWIRE::ONEWIRE(gpio_num_t p)
{
    DEBUG("ONEWIRE: construtor\n");
    PIN_DADOS = p;
    high();
}

void ONEWIRE::config(gpio_num_t pino)
{
    PIN_DADOS = pino;
    high();
}

void ONEWIRE::low(void)
{
    digital.pinMode(PIN_DADOS, OUTPUT);
    digital.digitalWrite(PIN_DADOS, LOW);
}

void ONEWIRE::high(void)
{
    digital.pinMode(PIN_DADOS, INPUT);
    gpio_set_pull_mode(PIN_DADOS, GPIO_PULLUP_ONLY);
}

uint8_t ONEWIRE::reset(void)
{
    uint8_t resposta;

    low();
    delay_us(480);

    high();
    delay_us(70);

    resposta = digital.digitalRead(PIN_DADOS);

    delay_us(400);
    return resposta;
}

void ONEWIRE::escreve_bit(uint8_t bit)
{
    if (bit)
    {
        low();
        delay_us(5);
        high();
        delay_us(55);
    }
    else
    {
        low();
        delay_us(60);
        high();
        delay_us(5);
    }
}

void ONEWIRE::writeByte(uint8_t v)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        escreve_bit((v >> i) & 0x01);
    }
}

uint8_t ONEWIRE::readBit(void)
{
    uint8_t c;

    low();
    delay_us(2);

    high();
    delay_us(11);

    c = digital.digitalRead(PIN_DADOS);

    delay_us(48);
    return c;
}

uint8_t ONEWIRE::readByte(void)
{
    uint8_t valor = 0;

    for (uint8_t x = 0; x < 8; x++)
    {
        uint8_t c;

        low();
        delay_us(2);

        high();
        delay_us(11);

        c = digital.digitalRead(PIN_DADOS);

        valor |= (c & 0x01) << x;

        delay_us(48);
    }

    return valor;
}