#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "WiFiManager.h"
#include "MQTTManager.h"
#include "TempSensor.h"

static const char *WIFI_SSID = "SEU_WIFI";
static const char *WIFI_PASS = "SUA_SENHA";
static const char *MQTT_BROKER = "mqtt://broker.emqx.io";

extern "C" void app_main(void)
{
    WiFiManager wifi(WIFI_SSID, WIFI_PASS);
    TempSensor sensor;
    MQTTManager mqtt(MQTT_BROKER, &sensor);

    wifi.begin();
    wifi.waitUntilConnected();

    mqtt.begin();

    while (true)
    {
        mqtt.loop();
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}