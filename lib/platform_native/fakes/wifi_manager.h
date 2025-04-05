#include "wifi/wifi_manager.h"

class FakeWiFiManager : public WiFiManager
{
public:
    bool connectCalled = false;
    int attempts = 0;
    bool simulateConnectionSuccess = true;

    bool connect(const char *ssid, const char *password) override
    {
        connectCalled = true;
        attempts++;
        return simulateConnectionSuccess;
    }

    bool isConnected() override
    {
        return simulateConnectionSuccess;
    }
};