#include "MQTTManager.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "esp_log.h"
#include "DS18B20Sensor.h"

static const char *TAG = "MQTTManager";

MQTTManager::MQTTManager(const char *brokerUri, DS18B20Sensor *sensor)
    : client(nullptr),
      brokerUri(brokerUri),
      sensor(sensor),
      lowLimit(20.0f),
      highLimit(30.0f),
      mqttConnected(false)
{
}

void MQTTManager::begin()
{
    ESP_LOGI(TAG, "Inicializando cliente MQTT...");
    ESP_LOGI(TAG, "Broker: %s", brokerUri);

    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = brokerUri;

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, &MQTTManager::mqttEventHandler, this);
    esp_mqtt_client_start(client);
}

void MQTTManager::mqttEventHandler(
    void *handler_args,
    esp_event_base_t base,
    int32_t event_id,
    void *event_data)
{
    auto *self = static_cast<MQTTManager *>(handler_args);
    auto event = static_cast<esp_mqtt_event_handle_t>(event_data);

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT conectado ao broker.");
        self->mqttConnected = true;

        esp_mqtt_client_subscribe(self->client, "/configura/alta", 0);
        esp_mqtt_client_subscribe(self->client, "/configura/baixa", 0);
        esp_mqtt_client_subscribe(self->client, "/informa/temperaturaCorrente", 0);
        esp_mqtt_client_subscribe(self->client, "/informa/limiteAlta", 0);
        esp_mqtt_client_subscribe(self->client, "/informa/limiteBaixa", 0);

        ESP_LOGI(TAG, "Topicos MQTT assinados.");
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "MQTT desconectado.");
        self->mqttConnected = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "Subscribe confirmado. msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "Mensagem MQTT recebida.");
        self->handleData(event->topic, event->topic_len, event->data, event->data_len);
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "Erro no MQTT.");
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

    ESP_LOGI(TAG, "Topico recebido: %s", topicBuffer);
    ESP_LOGI(TAG, "Payload recebido: %s", dataLen > 0 ? dataBuffer : "(sem payload)");

    if (std::strcmp(topicBuffer, "/configura/alta") == 0)
    {
        highLimit = std::strtof(dataBuffer, nullptr);
        ESP_LOGI(TAG, "Novo limite alto configurado: %.2f", highLimit);
    }
    else if (std::strcmp(topicBuffer, "/configura/baixa") == 0)
    {
        lowLimit = std::strtof(dataBuffer, nullptr);
        ESP_LOGI(TAG, "Novo limite baixo configurado: %.2f", lowLimit);
    }
    else if (std::strcmp(topicBuffer, "/informa/temperaturaCorrente") == 0)
    {
        float temp = sensor->read();
        ESP_LOGI(TAG, "Respondendo temperatura corrente: %.2f", temp);
        publishTemperature("/responde/temperaturaCorrente", temp);
    }
    else if (std::strcmp(topicBuffer, "/informa/limiteAlta") == 0)
    {
        ESP_LOGI(TAG, "Respondendo limite alto: %.2f", highLimit);
        publishLimit("/responde/limiteAlta", highLimit);
    }
    else if (std::strcmp(topicBuffer, "/informa/limiteBaixa") == 0)
    {
        ESP_LOGI(TAG, "Respondendo limite baixo: %.2f", lowLimit);
        publishLimit("/responde/limiteBaixa", lowLimit);
    }
}

void MQTTManager::publishTemperature(const char *topic, float value)
{
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%.2f", value);
    esp_mqtt_client_publish(client, topic, buffer, 0, 1, 0);
    ESP_LOGI(TAG, "Publicado em %s: %s", topic, buffer);
}

void MQTTManager::publishLimit(const char *topic, float value)
{
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%.2f", value);
    esp_mqtt_client_publish(client, topic, buffer, 0, 1, 0);
    ESP_LOGI(TAG, "Publicado em %s: %s", topic, buffer);
}

void MQTTManager::loop()
{
    if (!mqttConnected)
    {
        return;
    }

    float temp = sensor->read();
    ESP_LOGI(TAG, "Temperatura atual: %.2f", temp);

    if (temp > highLimit)
    {
        ESP_LOGW(TAG, "Temperatura acima do limite alto.");
        publishTemperature("/alerta/temperaturaAlta", temp);
    }
    else if (temp < lowLimit)
    {
        ESP_LOGW(TAG, "Temperatura abaixo do limite baixo.");
        publishTemperature("/alerta/temperaturaBaixa", temp);
    }
}