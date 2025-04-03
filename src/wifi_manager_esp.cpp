#include "wifi_manager_esp.h"
#include <WiFi.h>

bool RealWiFiManager::connect(const char *ssid, const char *password)
{
    WiFi.begin(ssid, password);
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 10)
    {
        delay(1000);
        retries++;
    }
    return WiFi.status() == WL_CONNECTED;
}

bool RealWiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}