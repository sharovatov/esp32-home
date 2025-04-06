#include <Arduino.h>
#include <WiFi.h>
#include <unity.h>

#include "credentials.h"
#include "wifi/wifi_manager.h"
#include "wifi_manager_esp.h"
#include "mqtt/mqtt_client.h"
#include "mqtt_client_esp.h"

#include "dht_sensor_esp.h"
#define DHTPIN 33
#include "temperature_sensor_esp.h"
#include "humidity_sensor_esp.h"

WiFiClient wifiClient;
bool mqttMessageReceived = false;
String mqttReceivedTopic;
String mqttReceivedMessage;

void mqttCallback(char *topic, uint8_t *payload, unsigned int length)
{
    mqttReceivedTopic = String(topic);
    mqttReceivedMessage = "";
    for (unsigned int i = 0; i < length; ++i)
    {
        mqttReceivedMessage += (char)payload[i];
    }
    mqttMessageReceived = true;
}

// === Helpers ===

bool connectToWiFi()
{
    RealWiFiManager wifi;
    return wifi.connect(WIFI_SSID, WIFI_PASSWORD);
}

RealMqttClient createConnectedMqttClient()
{
    RealMqttClient mqtt(wifiClient, MQTT_BROKER_HOST, MQTT_BROKER_PORT);
    mqtt.setCallback(mqttCallback);

    bool connected = mqtt.connect("esp32_test_client", MQTT_USERNAME, MQTT_PASSWORD);
    TEST_ASSERT_TRUE_MESSAGE(connected, "MQTT connection failed");

    return mqtt;
}

void cleanUp()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

// === Tests ===

void test_wifi_connects_successfully()
{
    TEST_ASSERT_TRUE(connectToWiFi());
    cleanUp();
}

void test_mqtt_publish_and_receive()
{
    TEST_ASSERT_TRUE(connectToWiFi());

    RealMqttClient mqtt = createConnectedMqttClient();

    // subscribe to a non-important topic
    String topic = "esp32/test/topic";
    mqtt.subscribe(topic.c_str());

    delay(500); // give broker time to process subscription

    String msg = "hello from esp integration test";
    mqtt.publish(topic.c_str(), msg.c_str());

    mqttMessageReceived = false;
    mqttReceivedTopic = "";
    mqttReceivedMessage = "";

    int attempts = 0;
    while (!mqttMessageReceived && attempts++ < 30)
    {
        mqtt.loop();
        delay(100);
    }

    TEST_ASSERT_TRUE_MESSAGE(mqttMessageReceived, "MQTT message was not received");
    TEST_ASSERT_EQUAL_STRING(topic.c_str(), mqttReceivedTopic.c_str());
    TEST_ASSERT_EQUAL_STRING(msg.c_str(), mqttReceivedMessage.c_str());

    cleanUp();
}

// test dht11 for real readings
void test_temperature_sensor_reads_value()
{
    TemperatureSensor sensor(DHTPIN, DHT11, "temp");
    auto reading = sensor.read();
    TEST_ASSERT_TRUE_MESSAGE(strcmp("error", reading.c_str()) != 0, "TemperatureSensor returned 'error'");
}
void test_humidity_sensor_reads_value()
{
    HumiditySensor sensor(DHTPIN, DHT11, "humidity");
    auto reading = sensor.read();
    Serial.print("Humidity reading: ");
    Serial.println(reading.c_str());
    TEST_ASSERT_TRUE_MESSAGE(strcmp("error", reading.c_str()) != 0, "HumiditySensor returned 'error'");
}

// I'd love to do an integration test for the buzzer but the code can't listen lol

void setup()
{
    delay(2000); // Wait for serial monitor to connect
    UNITY_BEGIN();
    RUN_TEST(test_temperature_sensor_reads_value);
    delay(100); // needed for dht11 since it's slow
    RUN_TEST(test_humidity_sensor_reads_value);
    RUN_TEST(test_wifi_connects_successfully);
    RUN_TEST(test_mqtt_publish_and_receive);

    UNITY_END();
}

void loop() {}