#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include "mqtt_client.h"

class DS18B20Sensor;

class MQTTManager
{
public:
    MQTTManager(const char *brokerUri, DS18B20Sensor *sensor);

    void begin();
    void loop();

    void setLimits(float low, float high);
    float getLowLimit() const;
    float getHighLimit() const;

private:
    static void mqttEventHandler(void *handler_args,
                                 esp_event_base_t base,
                                 int32_t event_id,
                                 void *event_data);

    void handleData(const char *topic, int topicLen, const char *data, int dataLen);
    void publishTemperature(const char *topic, float value);
    void publishLimit(const char *topic, float value);

    esp_mqtt_client_handle_t client;
    const char *brokerUri;
    DS18B20Sensor *sensor;
    float lowLimit;
    float highLimit;
    bool mqttConnected;
};

#endif