// ========== ESP libs ==========
#include <Arduino.h>
#include <WiFi.h>

// ========== My libs ==========
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

// ========== Sensors ==========
#include "buzzer_esp.h"
#include "camera_sensor_esp.h"

#define DHTPIN 33
#include "temperature_sensor_esp.h"
#include "humidity_sensor_esp.h"
#include <air_quality_sensor.h>

// ========== Globals ==========
WiFiClient wifiClient;
RealMqttClient *mqtt = nullptr;
SensorRegistry registry;

// ========== sensors init  ==========
std::vector<std::shared_ptr<ISensor>> createSensors()
{
  auto camera = std::make_shared<RealCameraSensor>();
  camera->init();

  return {
      std::make_shared<TemperatureSensor>(DHTPIN, DHT11, "temp"),
      std::make_shared<HumiditySensor>(DHTPIN, DHT11, "humidity"),
      std::make_shared<MQ135Sensor>(13, 30, 230),
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