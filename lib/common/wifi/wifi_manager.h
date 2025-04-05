#pragma once

class WiFiManager
{
public:
    virtual bool connect(const char *ssid, const char *password) = 0;
    virtual bool isConnected() = 0;
    virtual ~WiFiManager() {}
};

bool tryConnectWithRetry(WiFiManager &wifi, const char *ssid, const char *password, int maxAttempts = 3);