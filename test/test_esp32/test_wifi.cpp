#include <Arduino.h>
#include <unity.h>
#include <WiFi.h>
#include "wifi_manager.h"
#include "../../src/wifi_manager.cpp"
#include "credentials.h"

class RealWiFiManager : public WiFiManager
{
public:
    bool connect(const char *ssid, const char *password) override
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

    bool isConnected() override
    {
        return WiFi.status() == WL_CONNECTED;
    }
};

void test_wifi_connects_successfully()
{
    RealWiFiManager wifi;
    bool result = wifi.connect(WIFI_SSID, WIFI_PASSWORD);
    TEST_ASSERT_TRUE(result);
}

void setup()
{
    // This runs once when the test starts
    delay(2000); // wait for serial monitor to connect
    UNITY_BEGIN();
    RUN_TEST(test_wifi_connects_successfully);
    UNITY_END();
}

void loop()
{
    // Leave empty
}