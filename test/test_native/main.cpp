#include <unity.h>
#include <vector>
#include <string>

#include "sensor_registry.h"
#include "isensor.h"
#include "../../src/sensor_registry.cpp"
#include "sensor_request_handler.h"
#include "../../src/sensor_request_handler.cpp"
#include "sensor_publisher.h"
#include "../../src/sensor_publisher.cpp"

#include "fake_mqtt_client.h"
#include "mqtt_dispatcher.h"
#include "../../src/mqtt_dispatcher.cpp"

#include "boot.h"
#include "../../src/boot.cpp"

#include "wifi_manager.h"
#include "../../src/wifi_manager.cpp"

// sensor test double
class DummySensor : public ISensor
{
public:
    DummySensor(std::string sensorName, std::string result)
        : sensorName(sensorName), result(result) {}

    std::string name() const override { return sensorName; }
    std::string read() const override { return result; }

private:
    std::string sensorName;
    std::string result;
};

/*
  I want the sensors registry to be populated with all the physically present sensors,
  so that later on, when I add another sensor to the board, I will just add another
  sensor to the registry, and also add supporting code (driver)

*/
void test_registry_returns_available_sensor_names()
{
    SensorRegistry registry;
    registry.add(std::make_shared<DummySensor>("camera", "dummy"));
    registry.add(std::make_shared<DummySensor>("temp", "dummy"));
    registry.add(std::make_shared<DummySensor>("humidity", "dummy"));

    auto names = registry.listNames();
    TEST_ASSERT_EQUAL(3, names.size());
    TEST_ASSERT_EQUAL_STRING("camera", names[0].c_str());
    TEST_ASSERT_EQUAL_STRING("temp", names[1].c_str());
    TEST_ASSERT_EQUAL_STRING("humidity", names[2].c_str());
}

// all sensors from the registry should be publisheable to mqtt
void test_publishes_available_sensor_names_to_mqtt()
{
    SensorRegistry registry;
    registry.add(std::make_shared<DummySensor>("camera", ""));
    registry.add(std::make_shared<DummySensor>("temp", ""));
    registry.add(std::make_shared<DummySensor>("humidity", ""));

    FakeMqttClient mqtt;
    publishAvailableSensors(registry, mqtt);

    TEST_ASSERT_EQUAL_STRING("esp32/available_sensors", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING("[\"camera\",\"temp\",\"humidity\"]", mqtt.lastMessage.c_str());
}

// if a request comes in for a sensor that does not exist in the registry,
// the system should publish an error message to esp32/response/error.
void test_unknown_sensor_publishes_error()
{
    SensorRegistry registry;
    registry.add(std::make_shared<DummySensor>("camera", "imagebytes"));

    FakeMqttClient mqtt;

    std::string nonexistingSensor = "nonexistent";
    std::string expectedErrorMsg = "sensor_unknown:" + nonexistingSensor;

    handleSensorRequest(nonexistingSensor, registry, mqtt);

    TEST_ASSERT_EQUAL_STRING("esp32/response/error", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING(expectedErrorMsg.c_str(), mqtt.lastMessage.c_str());
}

// sensors should be able to respond to requests correctly with the readings
void test_sensor_request_is_routed_and_published()
{
    SensorRegistry registry;
    registry.add(std::make_shared<DummySensor>("camera", "imagebytes"));

    FakeMqttClient mqtt;

    handleSensorRequest("camera", registry, mqtt);

    TEST_ASSERT_EQUAL_STRING("esp32/response/camera", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING("imagebytes", mqtt.lastMessage.c_str());
}

// requests routing should work correctly, a message published to esp32/request/temp
// should route to the temp sensor
void test_request_topic_dispatches_to_sensor()
{
    SensorRegistry registry;
    registry.add(std::make_shared<DummySensor>("temp", "22.5"));

    FakeMqttClient mqtt;

    dispatchMqttRequest("esp32/request/temp", registry, mqtt);

    TEST_ASSERT_EQUAL_STRING("esp32/response/temp", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING("22.5", mqtt.lastMessage.c_str());
}

// request routing should fail gracefully by publishing an error if the request is invalid
void test_invalid_mqtt_topic_publishes_error()
{
    SensorRegistry registry;
    FakeMqttClient mqtt;

    std::string invalidTopic = "bad/request/temp";
    dispatchMqttRequest(invalidTopic, registry, mqtt);

    TEST_ASSERT_EQUAL_STRING("esp32/response/error", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING(("invalid_topic:" + invalidTopic).c_str(), mqtt.lastMessage.c_str());
}

// routing should also fail for the correct request but no sensor name
void test_empty_sensor_name_publishes_error()
{
    SensorRegistry registry;
    FakeMqttClient mqtt;

    std::string emptySensorTopic = "esp32/request/";
    dispatchMqttRequest(emptySensorTopic, registry, mqtt);

    TEST_ASSERT_EQUAL_STRING("esp32/response/error", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING("sensor_unknown:", mqtt.lastMessage.c_str());
}

// =============== boot ===============

// bootSystem function should register all sensors in the registry and publish them to mqtt
void test_boot_system_registers_and_publishes_sensors()
{
    SensorRegistry registry;
    FakeMqttClient mqtt;

    std::vector<std::shared_ptr<ISensor>> sensors = {
        std::make_shared<DummySensor>("camera", "dummy"),
        std::make_shared<DummySensor>("temp", "dummy"),
        std::make_shared<DummySensor>("humidity", "dummy"),
    };

    bootSystem(sensors, registry, mqtt);

    TEST_ASSERT_EQUAL_STRING("esp32/available_sensors", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING("[\"camera\",\"temp\",\"humidity\"]", mqtt.lastMessage.c_str());
}

// =============== wifi ===============

// wifi test double
class FakeWiFiManager : public WiFiManager
{
public:
    bool connectCalled = false;
    int attempts = 0;
    bool simulateConnectionSuccess = true;

    bool connect(const char *ssid, const char *password) override
    {
        connectCalled = true;
        attempts++;
        return simulateConnectionSuccess;
    }

    bool isConnected() override
    {
        return simulateConnectionSuccess;
    }
};

// check that the connection can succeed
void test_wifi_connection_succeeds_on_first_attempt()
{
    FakeWiFiManager wifi;
    wifi.simulateConnectionSuccess = true;

    bool result = tryConnectWithRetry(wifi, "testssid", "testpwd");

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(1, wifi.attempts);
}

// test that retries are supported and the connection fails after the limit is reached
void test_wifi_connection_retries_and_fails_after_3_attempts()
{
    FakeWiFiManager wifi;
    wifi.simulateConnectionSuccess = false;

    bool result = tryConnectWithRetry(wifi, "testssid", "testpwd");

    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL(3, wifi.attempts);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_registry_returns_available_sensor_names);
    RUN_TEST(test_publishes_available_sensor_names_to_mqtt);
    RUN_TEST(test_sensor_request_is_routed_and_published);
    RUN_TEST(test_unknown_sensor_publishes_error);
    RUN_TEST(test_request_topic_dispatches_to_sensor);
    RUN_TEST(test_invalid_mqtt_topic_publishes_error);
    RUN_TEST(test_empty_sensor_name_publishes_error);
    RUN_TEST(test_boot_system_registers_and_publishes_sensors);
    RUN_TEST(test_wifi_connection_succeeds_on_first_attempt);
    RUN_TEST(test_wifi_connection_retries_and_fails_after_3_attempts);

    return UNITY_END();
}