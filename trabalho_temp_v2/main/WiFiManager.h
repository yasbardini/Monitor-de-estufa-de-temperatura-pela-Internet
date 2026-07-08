#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"

class WiFiManager
{
public:
    WiFiManager(const char *ssid, const char *password);
    void begin();
    void waitUntilConnected();

private:
    static void eventHandler(
        void *arg,
        esp_event_base_t event_base,
        int32_t event_id,
        void *event_data);

    const char *ssid;
    const char *password;

    static EventGroupHandle_t wifiEventGroup;
    static const int WIFI_CONNECTED_BIT = BIT0;
};

#endif