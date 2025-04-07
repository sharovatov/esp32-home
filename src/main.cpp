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

#define DHTPIN 33
#include "temperature_sensor_esp.h"
#include "humidity_sensor_esp.h"

#include "camera_sensor_esp.h"

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

// ========== sensors init  ==========
std::vector<std::shared_ptr<ISensor>> createSensors()
{
  auto camera = std::make_shared<RealCameraSensor>();
  camera->init();

  return {
      std::make_shared<TemperatureSensor>(DHTPIN, DHT11, "temp"),
      std::make_shared<HumiditySensor>(DHTPIN, DHT11, "humidity"),
      camera};
}

// ========== MQTT Callback ==========
void mqttCallback(char *topic, uint8_t *payload, unsigned int length)
{
  std::string topicStr(topic);
  std::string payloadStr(reinterpret_cast<char *>(payload), length);

  Buzzer buzzer(14);
  dispatchMqttRequest(topicStr, registry, *mqtt, buzzer);
}

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
  mqtt->subscribe("esp32/request/+");

  bootSystem(createSensors(), registry, *mqtt);
}

// main loop
void loop()
{
  if (mqtt)
  {
    mqtt->loop();
  }

  delay(100);
}