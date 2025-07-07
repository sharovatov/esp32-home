#include <Arduino.h>
#include <WiFi.h>
#include <unity.h>

#include "credentials.h"
#include "wifi/wifi_manager.h"
#include "wifi_manager_esp.h"
#include "mqtt/mqtt_client.h"
#include "mqtt_client_esp.h"

#define DHTPIN 33
#include "temperature_sensor_esp.h"
#include "humidity_sensor_esp.h"
#include <camera_sensor_esp.h>

#include "air_quality_sensor.h"
#define MQ135PIN 32

WiFiClient wifiClient;
bool mqttMessageReceived = false;
String mqttReceivedTopic;
String mqttReceivedMessage;

#include <ctime>
#include "qase_reporter.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

struct EspHttpClient : public qase::HttpClient
{
    std::string post(const std::string &url,
                     const std::string &payload,
                     const std::vector<std::string> &headers) override
    {
        if (url.rfind("https://", 0) != 0)
        {
            throw std::runtime_error("Only https:// URLs are supported");
        }

        HTTPClient http;
        http.begin(url.c_str()); // Automatically handles HTTPS via WiFiClientSecure internally

        for (const auto &h : headers)
        {
            auto sep = h.find(':');
            if (sep != std::string::npos)
            {
                auto key = h.substr(0, sep);
                auto val = h.substr(sep + 1);
                http.addHeader(key.c_str(), val.c_str());
            }
        }

        int httpCode = http.POST(payload.c_str());
        if (httpCode < 0)
        {
            http.end();
            throw std::runtime_error("HTTP POST failed: " + std::string(http.errorToString(httpCode).c_str()));
        }

        String response = http.getString();
        http.end();

        return std::string(response.c_str());
    }
};

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
    TemperatureSensor sensor(DHTPIN, DHT11);
    auto reading = sensor.read();
    TEST_ASSERT_TRUE_MESSAGE(strcmp("error", reading.c_str()) != 0, "TemperatureSensor returned 'error'");
}
void test_humidity_sensor_reads_value()
{
    HumiditySensor sensor(DHTPIN, DHT11);
    auto reading = sensor.read();
    Serial.print("Humidity reading: ");
    Serial.println(reading.c_str());
    TEST_ASSERT_TRUE_MESSAGE(strcmp("error", reading.c_str()) != 0, "HumiditySensor returned 'error'");
}

// I'd love to do an integration test for the buzzer but the code can't listen lol

// test the camera if it returns something like base64
void test_camera_returns_base64_string()
{
    RealCameraSensor camera;
    camera.init();
    std::string image = camera.read();

    TEST_ASSERT_FALSE_MESSAGE(image.empty(), "Camera returned empty string");
    TEST_ASSERT_TRUE_MESSAGE(image.length() > 10000, "Camera output too small to be a valid JPEG in base64");
}

void test_mq135_sensor_reads_percentage()
{
    MQ135Sensor sensor(MQ135PIN, 30, 230);

    std::string reading = sensor.read();
    Serial.print("MQ135 reading: ");
    Serial.println(reading.c_str());

    int value = std::stoi(reading);
    TEST_ASSERT_TRUE_MESSAGE(value >= 0 && value <= 100, "MQ135 percentage out of range");
}

void setup()
{
    Serial.begin(115200);
    delay(2000); // Wait for serial monitor to connect
    QASE_UNITY_BEGIN();

    QASE_RUN_TEST(test_temperature_sensor_reads_value);
    delay(100); // needed for dht11 since it's slow
    QASE_RUN_TEST(test_humidity_sensor_reads_value);
    QASE_RUN_TEST(test_wifi_connects_successfully);
    QASE_RUN_TEST(test_mqtt_publish_and_receive);
    QASE_RUN_TEST(test_camera_returns_base64_string);
    QASE_RUN_TEST(test_mq135_sensor_reads_percentage);

    Serial.println("MAIN.CPP: tests are run, now we need to finish");

    EspHttpClient http;

    connectToWiFi();

    Serial.println("MAIN.CPP: connected to wifi, will now init QaseConfig");

    qase::QaseConfig cfg;
    cfg.token = "4a02e17acfa32e7b71067e3beb597490f8a9bda427697c2a3bf49044582ee668";
    cfg.project = "ET1";
    cfg.host = "api.qase.io";
    cfg.run_title = "e2e tests";

    qase::ConfigResolutionInput input;
    input.preset = cfg;

    cfg = resolve_config(input);

    Serial.println("MAIN.CPP: config inited, will call QASE_UNITY_END now");

    try
    {
        QASE_UNITY_END(http, cfg);
    }
    catch (const std::exception &e)
    {
        Serial.println("Qase reporter failed:");
        Serial.println(e.what());
    }
}

void loop() {}
