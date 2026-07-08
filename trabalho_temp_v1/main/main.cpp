#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "WiFiManager.h"
#include "MQTTManager.h"
#include "TempSensor.h"

static const char *TAG = "MAIN";

static const char *WIFI_SSID = "AP 02";
static const char *WIFI_PASS = "jardimdasavenidas";
static const char *MQTT_BROKER = "mqtt://broker.emqx.io";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "app_main iniciado.");

    WiFiManager wifi(WIFI_SSID, WIFI_PASS);
    TempSensor sensor;
    MQTTManager mqtt(MQTT_BROKER, &sensor);

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