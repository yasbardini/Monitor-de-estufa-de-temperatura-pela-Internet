#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include "WiFiManager.h"
#include "MQTTManager.h"
#include "DS18B20Sensor.h"

static const char *TAG = "MAIN";

static const char *WIFI_SSID = "AP 02";
static const char *WIFI_PASS = "jardimdasavenidas";
static const char *MQTT_BROKER = "mqtt://broker.emqx.io";
static const gpio_num_t DS18B20_GPIO = GPIO_NUM_13;

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "app_main iniciado.");

    WiFiManager wifi(WIFI_SSID, WIFI_PASS);
    DS18B20Sensor sensor(DS18B20_GPIO);
    MQTTManager mqtt(MQTT_BROKER, &sensor);

    sensor.begin();

    wifi.begin();
    wifi.waitUntilConnected();

    ESP_LOGI(TAG, "Chamando mqtt.begin()...");
    mqtt.begin();
    ESP_LOGI(TAG, "mqtt.begin() chamado com sucesso.");

    while (true)
    {
        mqtt.loop();
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
