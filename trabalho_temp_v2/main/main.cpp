#include "freertos/FreeRTOS.h" //  que gerencia tarefas ao mesmo tempo
#include "freertos/task.h"
#include "driver/gpio.h" // controla o pino físico
#include "esp_log.h" // imprime mensagens de log

#include "WiFiManager.h" // gerencia a conexão Wi-Fi
#include "MQTTManager.h" // gerencia a conexão MQTT
#include "DS18B20Sensor.h" // gerencia o sensor de temperatura 

static const char *TAG = "MAIN"; // tag para identificar mensagens de log

static const char *WIFI_SSID = "NICANDA"; // pra conectar no wifi
static const char *WIFI_PASS = "Nicole2005!";
static const char *MQTT_BROKER = "mqtt://broker.emqx.io"; // endereço do broker MQTT
static const gpio_num_t DS18B20_GPIO = GPIO_NUM_16; // pino 16 conectado no sensor

extern "C" void app_main(void) // função principal do programa, idf foi escrita em c mas o código ta em c++
{git status
    ESP_LOGI(TAG, "app_main iniciado."); // imprime mensagem de log

    WiFiManager wifi(WIFI_SSID, WIFI_PASS); // cria objeto wifi com 
    DS18B20Sensor sensor(DS18B20_GPIO);  // cria objeto sensor com pino 16
    MQTTManager mqtt(MQTT_BROKER, &sensor); // cria objeto mqtt com endereço do broker e referência para o sensor

    sensor.begin(); // inicializa o sensor de temperatura

    wifi.begin(); // realiza a conexão Wi-Fi
    wifi.waitUntilConnected(); // espera até que a conexão Wi-Fi seja estabelecida 

    ESP_LOGI(TAG, "Chamando mqtt.begin()..."); // imprime log
    mqtt.begin(); // realiza a conexão com o protocolo MQTT
    ESP_LOGI(TAG, "mqtt.begin() chamado com sucesso.");

    while (true) // loop infinito para manter o programa em execução
    {
        mqtt.loop(); // mantém a conexão MQTT ativa e processa mensagens recebidas
        vTaskDelay(pdMS_TO_TICKS(5000)); // aguarda 5 segundos antes de enviar a próxima leitura de temperatura
    }
}
