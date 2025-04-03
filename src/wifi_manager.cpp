#include "wifi_manager.h"

bool tryConnectWithRetry(WiFiManager &wifi, const char *ssid, const char *password, int maxAttempts)
{
    for (int i = 0; i < maxAttempts; ++i)
    {
        if (wifi.connect(ssid, password))
        {
            return true;
        }
    }
    return false;
}