#pragma once
#include "wifi_manager.h"

// Concrete implementation for ESP-based WiFiManager
class RealWiFiManager : public WiFiManager
{
public:
    bool connect(const char *ssid, const char *password) override;
    bool isConnected() override;
};