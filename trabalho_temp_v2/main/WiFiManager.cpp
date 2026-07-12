#include "WiFiManager.h"

#include <cstring>

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

static const char *TAG = "WiFiManager"; // tag para identificar mensagens de log

EventGroupHandle_t WiFiManager::wifiEventGroup = nullptr; // grupo de eventos para gerenciar o estado da conexão Wi-Fi

WiFiManager::WiFiManager(const char *ssid, const char *password) // construtor da classe WiFiManager, que recebe o SSID e a senha da rede Wi-Fi como parâmetros
    : ssid(ssid), password(password)
{
}

void WiFiManager::eventHandler( // função de callback para lidar com eventos relacionados à conexão Wi-Fi
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) // verifica se o evento é de início da estação Wi-Fi
    {
        ESP_LOGI(TAG, "Wi-Fi iniciado, tentando conectar...");
        esp_wifi_connect(); // inicia a tentativa de conexão à rede Wi-Fi usando as credenciais fornecidas
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) // verifica se o evento é de desconexão da estação Wi-Fi
    {
        ESP_LOGW(TAG, "Wi-Fi desconectado, tentando reconectar...");
        xEventGroupClearBits(wifiEventGroup, WIFI_CONNECTED_BIT); // limpa o bit de conexão no grupo de eventos, indicando que a conexão Wi-Fi foi perdida
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) // verifica se o evento é de obtenção de endereço IP pela estação Wi-Fi
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data; // obtém os dados do evento, que contêm o endereço IP atribuído à estação Wi-Fi
        ESP_LOGI(TAG, "Wi-Fi conectado! IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifiEventGroup, WIFI_CONNECTED_BIT); // define o bit de conexão no grupo de eventos, indicando que a conexão Wi-Fi foi estabelecida com sucesso
    }
}

void WiFiManager::begin() // função que inicia o processo de conexão à rede Wi-Fi, configurando o adaptador Wi-Fi e registrando os manipuladores de eventos necessários
{
    ESP_LOGI(TAG, "Inicializando Wi-Fi..."); 

    esp_err_t ret = nvs_flash_init(); 
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    { // verifica se há erros de memória flash não volátil (NVS) que impedem a inicialização, como falta de páginas livres ou versão incompatível
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    wifiEventGroup = xEventGroupCreate(); // cria um grupo de eventos para gerenciar o estado da conexão Wi-Fi, permitindo que outras partes do código aguardem a conexão ser estabelecida antes de prosseguir

    ESP_ERROR_CHECK(esp_netif_init()); 
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register( 
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &WiFiManager::eventHandler,
        nullptr,
        &instance_any_id));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &WiFiManager::eventHandler,
        nullptr,
        &instance_got_ip));

    wifi_config_t wifi_config = {};
    std::strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    std::strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
} 

void WiFiManager::waitUntilConnected() // função que aguarda até que a conexão Wi-Fi seja estabelecida, bloqueando a execução do programa até que o bit de conexão seja definido no grupo de eventos
{
    ESP_LOGI(TAG, "Aguardando conexão Wi-Fi...");
    xEventGroupWaitBits(
        wifiEventGroup,
        WIFI_CONNECTED_BIT,
        pdFALSE,
        pdTRUE,
        portMAX_DELAY);
    ESP_LOGI(TAG, "Conexão Wi-Fi confirmada.");
}