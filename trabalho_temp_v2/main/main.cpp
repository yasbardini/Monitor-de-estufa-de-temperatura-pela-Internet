#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "WiFiManager.h"
#include "MQTTManager.h"
#include "DS18B20Sensor.h"

static const char *WIFI_SSID = "yaslinda";
static const char *WIFI_PASS = "98140221";
static const char *MQTT_BROKER = "mqtt://broker.emqx.io";

extern "C" void app_main(void)
{
    WiFiManager wifi(WIFI_SSID, WIFI_PASS);
    DS18B20Sensor sensor(GPIO_NUM_4);
    MQTTManager mqtt(MQTT_BROKER, &sensor);

    wifi.begin();
    wifi.waitUntilConnected();

    sensor.begin();
    mqtt.begin();

    while (true)
    {
        mqtt.loop();
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}