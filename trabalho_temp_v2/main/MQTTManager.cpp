#include "MQTTManager.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "DS18B20Sensor.h"

MQTTManager::MQTTManager(const char *brokerUri, DS18B20Sensor *sensor)
    : client(nullptr),
      brokerUri(brokerUri),
      sensor(sensor),
      lowLimit(20.0f),
      highLimit(30.0f),
      mqttConnected(false) {}

void MQTTManager::begin()
{
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = brokerUri;

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, &MQTTManager::mqttEventHandler, this);
    esp_mqtt_client_start(client);
}

void MQTTManager::mqttEventHandler(void *handler_args,
                                   esp_event_base_t base,
                                   int32_t event_id,
                                   void *event_data)
{
    auto *self = static_cast<MQTTManager *>(handler_args);
    auto event = static_cast<esp_mqtt_event_handle_t>(event_data);

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        self->mqttConnected = true;
        esp_mqtt_client_subscribe(self->client, "/configura/alta", 0);
        esp_mqtt_client_subscribe(self->client, "/configura/baixa", 0);
        esp_mqtt_client_subscribe(self->client, "/informa/temperaturaCorrente", 0);
        esp_mqtt_client_subscribe(self->client, "/informa/limiteAlta", 0);
        esp_mqtt_client_subscribe(self->client, "/informa/limiteBaixa", 0);
        break;

    case MQTT_EVENT_DISCONNECTED:
        self->mqttConnected = false;
        break;

    case MQTT_EVENT_DATA:
        self->handleData(event->topic, event->topic_len, event->data, event->data_len);
        break;

    default:
        break;
    }
}

void MQTTManager::handleData(const char *topic, int topicLen, const char *data, int dataLen)
{
    char topicBuffer[128] = {0};
    char dataBuffer[128] = {0};

    std::memcpy(topicBuffer, topic, topicLen);
    topicBuffer[topicLen] = '\0';

    if (data != nullptr && dataLen > 0)
    {
        std::memcpy(dataBuffer, data, dataLen);
        dataBuffer[dataLen] = '\0';
    }

    if (std::strcmp(topicBuffer, "/configura/alta") == 0)
    {
        highLimit = std::strtof(dataBuffer, nullptr);
        publishLimit("/responde/limiteAlta", highLimit);
    }
    else if (std::strcmp(topicBuffer, "/configura/baixa") == 0)
    {
        lowLimit = std::strtof(dataBuffer, nullptr);
        publishLimit("/responde/limiteBaixa", lowLimit);
    }
    else if (std::strcmp(topicBuffer, "/informa/temperaturaCorrente") == 0)
    {
        publishTemperature("/responde/temperaturaCorrente", sensor->read());
    }
    else if (std::strcmp(topicBuffer, "/informa/limiteAlta") == 0)
    {
        publishLimit("/responde/limiteAlta", highLimit);
    }
    else if (std::strcmp(topicBuffer, "/informa/limiteBaixa") == 0)
    {
        publishLimit("/responde/limiteBaixa", lowLimit);
    }
}

void MQTTManager::publishTemperature(const char *topic, float value)
{
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%.2f", value);
    esp_mqtt_client_publish(client, topic, buffer, 0, 1, 0);
}

void MQTTManager::publishLimit(const char *topic, float value)
{
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%.2f", value);
    esp_mqtt_client_publish(client, topic, buffer, 0, 1, 0);
}

void MQTTManager::loop()
{
    if (!mqttConnected)
        return;

    float temp = sensor->read();

    if (temp == -127.0f)
    {
        return;
    }

    if (temp > highLimit)
    {
        publishTemperature("/alerta/temperaturaAlta", temp);
    }
    else if (temp < lowLimit)
    {
        publishTemperature("/alerta/temperaturaBaixa", temp);
    }
}

void MQTTManager::setLimits(float low, float high)
{
    lowLimit = low;
    highLimit = high;
}

float MQTTManager::getLowLimit() const
{
    return lowLimit;
}

float MQTTManager::getHighLimit() const
{
    return highLimit;
}