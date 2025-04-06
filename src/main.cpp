#include <Arduino.h>
#include <WiFi.h>

#include "credentials.h"
#include "wifi/wifi_manager.h"
#include "wifi_manager_esp.h"

#include "mqtt/mqtt_dispatcher.h"
#include "mqtt/mqtt_client.h"
#include "mqtt_client_esp.h"
#include "sensor/sensor_registry.h"
#include "sensor/sensor_request_handler.h"
#include "sensor/sensor_publisher.h"
#include "boot/boot.h"

#include "sensor/isensor.h"
#include "buzzer_esp.cpp"

// ========== Globals ==========
WiFiClient wifiClient;
RealMqttClient *mqtt = nullptr;
SensorRegistry registry;

// dummy sensor will only be used for some time here until it's not needed
class DummySensor : public ISensor
{
public:
  DummySensor(const std::string &n, const std::string &v) : sensorName(n), value(v) {}
  std::string name() const override { return sensorName; }
  std::string read() override { return value; }

private:
  std::string sensorName;
  std::string value;
};

// ========== MQTT Callback ==========
void mqttCallback(char *topic, uint8_t *payload, unsigned int length)
{
  std::string topicStr(topic);
  std::string payloadStr(reinterpret_cast<char *>(payload), length);

  Buzzer buzzer(14);
  dispatchMqttRequest(topicStr, registry, *mqtt, buzzer);
}

// ========== Setup ==========
void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("Booting device...");

  // Connect WiFi
  RealWiFiManager wifi;
  if (!wifi.connect(WIFI_SSID, WIFI_PASSWORD))
  {
    Serial.println("WiFi connection failed.");
    return;
  }
  Serial.println("Connected to WiFi.");

  // Init MQTT
  mqtt = new RealMqttClient(wifiClient, MQTT_BROKER_HOST, MQTT_BROKER_PORT);
  mqtt->setCallback(mqttCallback);
  if (!mqtt->connect("esp32_prod_client", MQTT_USERNAME, MQTT_PASSWORD))
  {
    Serial.println("MQTT connection failed.");
    return;
  }
  Serial.println("Connected to MQTT.");

  // Subscribe to sensor request topic
  mqtt->subscribe("esp32/request/+");

  // Boot system (register sensors + publish available)
  std::vector<std::shared_ptr<ISensor>> sensors = {
      std::make_shared<DummySensor>("temp", "23.4"),
      std::make_shared<DummySensor>("humidity", "60"),
      std::make_shared<DummySensor>("camera",
                                    "/9j/4AAQSkZJRgABAQEASABIAAD/2wBDAAMCAgMCAgMDAwMEAwMEBQgFBQQEBQoHBwYIDAoMDAsKCwsNDhIQDQ4RDgsLEBYQERMUFRUVDA8XGBYUGBIUFRT/wAALCAABAAEBAREA/8QAFAABAAAAAAAAAAAAAAAAAAAACf/EABQQAQAAAAAAAAAAAAAAAAAAAAD/2gAIAQEAAD8AKp//2Q==")};

  bootSystem(sensors, registry, *mqtt);
}

// ========== Loop ==========
void loop()
{
  if (mqtt)
  {
    mqtt->loop();
  }

  delay(100);
}