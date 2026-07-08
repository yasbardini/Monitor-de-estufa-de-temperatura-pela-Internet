#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <stdint.h>

#include "esp_event.h"
#include "mqtt_client.h"

class TempSensor;

class MQTTManager
{
public:
    MQTTManager(const char *brokerUri, TempSensor *sensor);

    void begin();
    void loop();

private:
    static void mqttEventHandler(
        void *handler_args,
        esp_event_base_t base,
        int32_t event_id,
        void *event_data);

    void handleData(const char *topic, int topicLen, const char *data, int dataLen);
    void publishTemperature(const char *topic, float value);
    void publishLimit(const char *topic, float value);

    esp_mqtt_client_handle_t client;
    const char *brokerUri;
    TempSensor *sensor;

    float lowLimit;
    float highLimit;
    bool mqttConnected;
};

#endif