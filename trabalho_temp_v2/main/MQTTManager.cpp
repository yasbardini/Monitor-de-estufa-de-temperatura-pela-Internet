#include "MQTTManager.h"

#include <cstdio> // inclui bib de entrada e saída padrão
#include <cstdlib>
#include <cstring>

#include "esp_log.h"
#include "DS18B20Sensor.h"

static const char *TAG = "MQTTManager"; // tag para identificar mensagens de log

MQTTManager::MQTTManager(const char *brokerUri, DS18B20Sensor *sensor) // construtor da classe MQTTManager, que recebe o URI do broker MQTT e uma referência para o sensor de temperatura DS18B20 como parâmetros
    : client(nullptr),
      brokerUri(brokerUri),
      sensor(sensor),
      lowLimit(20.0f), // valor inicial do limite baixo de temperatura
      highLimit(30.0f), // valor inicial do limite alto de temperatura
      mqttConnected(false)
{
}

void MQTTManager::begin() // função que inicia a conexão com o broker MQTT, configurando o cliente MQTT e registrando o manipulador de eventos necessário
{
    ESP_LOGI(TAG, "Inicializando cliente MQTT...");
    ESP_LOGI(TAG, "Broker: %s", brokerUri); // imprime o URI do broker MQTT no log para fins de depuração, uri do broker é o endereço de conexão

    esp_mqtt_client_config_t mqtt_cfg = {}; // estrutura de configuração do cliente MQTT, inicializada com valores padrão
    mqtt_cfg.broker.address.uri = brokerUri;

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, &MQTTManager::mqttEventHandler, this);
    esp_mqtt_client_start(client);
}

void MQTTManager::mqttEventHandler( // função de callback para lidar com eventos relacionados à conexão MQTT, como conexão, desconexão, recebimento de mensagens e erros
    void *handler_args,
    esp_event_base_t base,
    int32_t event_id,
    void *event_data)
{
    auto *self = static_cast<MQTTManager *>(handler_args); // 
    auto event = static_cast<esp_mqtt_event_handle_t>(event_data);

    switch ((esp_mqtt_event_id_t)event_id) // switch para tratar diferentes tipos de eventos MQTT
    {
    case MQTT_EVENT_CONNECTED: // evento de conexão com o broker MQTT
        ESP_LOGI(TAG, "MQTT conectado ao broker.");
        self->mqttConnected = true;
        // assina os tópicos MQTT relevantes para receber mensagens de configuração e informações do sensor
        esp_mqtt_client_subscribe(self->client, "/configura/alta", 0);
        esp_mqtt_client_subscribe(self->client, "/configura/baixa", 0);
        esp_mqtt_client_subscribe(self->client, "/informa/temperaturaCorrente", 0);
        esp_mqtt_client_subscribe(self->client, "/informa/limiteAlta", 0);
        esp_mqtt_client_subscribe(self->client, "/informa/limiteBaixa", 0);

        ESP_LOGI(TAG, "Topicos MQTT assinados.");
        break;

    case MQTT_EVENT_DISCONNECTED: // evento de desconexão do broker MQTT
        ESP_LOGW(TAG, "MQTT desconectado.");
        self->mqttConnected = false;
        break;

    case MQTT_EVENT_SUBSCRIBED: // evento de confirmação de assinatura de tópico MQTT
        ESP_LOGI(TAG, "Subscribe confirmado. msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA: //  evento de recebimento de mensagem MQTT em um tópico assinado
        ESP_LOGI(TAG, "Mensagem MQTT recebida.");
        self->handleData(event->topic, event->topic_len, event->data, event->data_len);
        break;

    case MQTT_EVENT_ERROR: // evento de erro na conexão MQTT
        ESP_LOGE(TAG, "Erro no MQTT.");
        break;

    default:
        break;
    }
}

void MQTTManager::handleData(const char *topic, int topicLen, const char *data, int dataLen) // função que processa os dados recebidos em um tópico MQTT, verificando o tópico e executando ações apropriadas com base no conteúdo da mensagem
{
    char topicBuffer[128] = {0};
    char dataBuffer[128] = {0};

    std::memcpy(topicBuffer, topic, topicLen); // copia o tópico recebido para um buffer local, garantindo que a string seja terminada com um caractere nulo para evitar problemas de leitura
    topicBuffer[topicLen] = '\0'; 

    if (data != nullptr && dataLen > 0) // verifica se há dados válidos recebidos no tópico MQTT antes de copiá-los para o buffer local, garantindo que a string seja terminada com um caractere nulo para evitar problemas de leitura
    {
        std::memcpy(dataBuffer, data, dataLen); // copia os dados recebidos para o buffer local
        dataBuffer[dataLen] = '\0';
    }

    ESP_LOGI(TAG, "Topico recebido: %s", topicBuffer);
    ESP_LOGI(TAG, "Payload recebido: %s", dataLen > 0 ? dataBuffer : "(sem payload)"); // exibe o payload recebido, ou uma mensagem indicando a ausência de payload
    // payload é o conteúdo da mensagem recebida no tópico MQTT, que pode conter informações de configuração ou solicitações de dados do sensor

    if (std::strcmp(topicBuffer, "/configura/alta") == 0) // verifica se o tópico recebido corresponde à configuração do limite alto de temperatura, e se for o caso, atualiza o valor do limite alto com base no payload recebido
    {
        highLimit = std::strtof(dataBuffer, nullptr);
        ESP_LOGI(TAG, "Novo limite alto configurado: %.2f", highLimit);
    }
    else if (std::strcmp(topicBuffer, "/configura/baixa") == 0) // verifica se o tópico recebido corresponde à configuração do limite baixo de temperatura, e se for o caso, atualiza o valor do limite baixo com base no payload recebido
    {
        lowLimit = std::strtof(dataBuffer, nullptr);
        ESP_LOGI(TAG, "Novo limite baixo configurado: %.2f", lowLimit);
    }
    else if (std::strcmp(topicBuffer, "/informa/temperaturaCorrente") == 0) // verifica se o tópico recebido corresponde à solicitação de informação da temperatura corrente, e se for o caso, lê a temperatura atual do sensor e publica a resposta no tópico apropriado
    {
        float temp = sensor->read();
        ESP_LOGI(TAG, "Respondendo temperatura corrente: %.2f", temp);
        publishTemperature("/responde/temperaturaCorrente", temp);
    }
    else if (std::strcmp(topicBuffer, "/informa/limiteAlta") == 0) // verifica se o tópico recebido corresponde à solicitação de informação do limite alto, e se for o caso, publica o valor atual do limite alto no tópico apropriado
    {
        ESP_LOGI(TAG, "Respondendo limite alto: %.2f", highLimit);
        publishLimit("/responde/limiteAlta", highLimit);
    }
    else if (std::strcmp(topicBuffer, "/informa/limiteBaixa") == 0) // verifica se o tópico recebido corresponde à solicitação de informação do limite baixo, e se for o caso, publica o valor atual do limite baixo no tópico apropriado

    {
        ESP_LOGI(TAG, "Respondendo limite baixo: %.2f", lowLimit);
        publishLimit("/responde/limiteBaixa", lowLimit);
    }
}

void MQTTManager::publishTemperature(const char *topic, float value) // função que publica a temperatura atual no tópico MQTT especificado, convertendo o valor da temperatura para uma string formatada antes de enviá-la ao broker MQTT  
// ela é chamada quando a temperatura atual é lida do sensor e precisa ser enviada para o broker MQTT, permitindo que outros dispositivos ou aplicativos recebam a informação da temperatura medida pelo sensor DS18B20
{
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%.2f", value);
    esp_mqtt_client_publish(client, topic, buffer, 0, 1, 0);
    ESP_LOGI(TAG, "Publicado em %s: %s", topic, buffer);
}

void MQTTManager::publishLimit(const char *topic, float value) // função que publica o valor do limite de temperatura (alto ou baixo) no tópico MQTT especificado, convertendo o valor do limite para uma string formatada antes de enviá-la ao broker MQTT
{
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%.2f", value); // converte o valor do limite de temperatura para uma string formatada com duas casas decimais, armazenando o resultado no buffer local
    esp_mqtt_client_publish(client, topic, buffer, 0, 1, 0); // publica a string formatada no tópico MQTT especificado, enviando a informação do limite de temperatura para o broker MQTT
    ESP_LOGI(TAG, "Publicado em %s: %s", topic, buffer);
}

void MQTTManager::loop() // função que mantém a conexão MQTT ativa e processa mensagens recebidas, além de verificar a temperatura atual do sensor e publicar alertas caso os limites de temperatura sejam ultrapassados
{
    if (!mqttConnected) // verifica se a conexão MQTT está ativa antes de tentar processar mensagens ou publicar alertas, garantindo que as operações relacionadas ao MQTT só sejam realizadas quando a conexão estiver estabelecida
    {
        return;
    }

    float temp = sensor->read(); // lê a temperatura atual do sensor DS18B20, obtendo o valor da temperatura medida pelo sensor para ser usado na verificação dos limites e na publicação de alertas caso necessário
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