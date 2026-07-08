#include "DS18B20Sensor.h"

#include "esp_log.h"

static const char *TAG = "DS18B20Sensor";

DS18B20Sensor::DS18B20Sensor(gpio_num_t pin)
    : pin(pin), sensor(pin), initialized(false)
{
}

void DS18B20Sensor::begin()
{
    ESP_LOGI(TAG, "Inicializando DS18B20 no GPIO %d...", (int)pin);

    sensor.init2();
    sensor.programa();

    initialized = true;
    ESP_LOGI(TAG, "DS18B20 inicializado.");
}

float DS18B20Sensor::read()
{
    if (!initialized)
    {
        ESP_LOGW(TAG, "Sensor nao inicializado. Chamando begin() automaticamente.");
        begin();
    }

    float temperatura = sensor.readTemp();
    ESP_LOGI(TAG, "Temperatura lida: %.2f", temperatura);

    return temperatura;
}