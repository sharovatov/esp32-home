#pragma once

// TODO: This will be replaced with a CellularManager when transitioning to SIM7670 3G module
class WiFiManager
{
public:
    virtual bool connect(const char *ssid, const char *password) = 0;
    virtual bool isConnected() = 0;
    virtual ~WiFiManager() {}
};

bool tryConnectWithRetry(WiFiManager &wifi, const char *ssid, const char *password, int maxAttempts = 3);