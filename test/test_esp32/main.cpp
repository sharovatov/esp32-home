#include <Arduino.h>
#include <unity.h>

// all the pwds
#include "credentials.h"

#include <WiFi.h>
#include "wifi/wifi_manager.h"
#include "wifi_manager_esp.h"

void test_wifi_connects_successfully()
{
    RealWiFiManager wifi;
    bool result = wifi.connect(WIFI_SSID, WIFI_PASSWORD);
    TEST_ASSERT_TRUE(result);
}

void setup()
{
    delay(2000); // wait for serial monitor to connect
    UNITY_BEGIN();
    RUN_TEST(test_wifi_connects_successfully);
    UNITY_END();

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

void loop()
{
    // empty for now
}