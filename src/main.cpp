#include <Arduino.h>

// all the pwds
#include "credentials.h"

#include <WiFi.h>
#include "wifi/wifi_manager.h"
#include "wifi_manager_esp.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("Booted, connecting to wifi...");
  RealWiFiManager wifi;
  wifi.connect(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("connected to wifi");
}

void loop()
{
  Serial.println("Still running...");
  delay(2000);
}