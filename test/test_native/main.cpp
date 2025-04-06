#include <unity.h>
#include <vector>
#include <string>

#include "sensor/sensor_registry.h"
#include "sensor/isensor.h"
#include "sensor/sensor_request_handler.h"
#include "sensor/sensor_publisher.h"

#include "fakes/mqtt_client.h"
#include "fakes/dummy_sensor.h"
#include "fakes/wifi_manager.h"
#include "fakes/dht_sensor.h"
#include "fakes/camera_sensor.h"

#include "sensor/temperature_sensor.h"
#include "sensor/humidity_sensor.h"

#include "mqtt/mqtt_dispatcher.h"

#include "boot/boot.h"
#include "fakes/buzzer_fake.h"
#include <HWCDC.h>
#include <HardwareSerial.h>

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

    FakeBuzzer buzzer;
    dispatchMqttRequest("esp32/request/temp", registry, mqtt, buzzer);

    TEST_ASSERT_EQUAL_STRING("esp32/response/temp", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING("22.5", mqtt.lastMessage.c_str());
}

// request routing should fail gracefully by publishing an error if the request is invalid
void test_invalid_mqtt_topic_publishes_error()
{
    SensorRegistry registry;
    FakeMqttClient mqtt;

    std::string invalidTopic = "bad/request/temp";

    FakeBuzzer buzzer;
    dispatchMqttRequest(invalidTopic, registry, mqtt, buzzer);

    TEST_ASSERT_EQUAL_STRING("esp32/response/error", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING(("invalid_topic:" + invalidTopic).c_str(), mqtt.lastMessage.c_str());
}

// routing should also fail for the correct request but no sensor name
void test_empty_sensor_name_publishes_error()
{
    SensorRegistry registry;
    FakeMqttClient mqtt;

    std::string emptySensorTopic = "esp32/request/";

    FakeBuzzer buzzer;
    dispatchMqttRequest(emptySensorTopic, registry, mqtt, buzzer);

    TEST_ASSERT_EQUAL_STRING("esp32/response/error", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING("sensor_unknown:", mqtt.lastMessage.c_str());
}

// test pwd/usrname working ok
// I know I'm only testing the fake, but this makes me not forget the pwd and usrname handling
void test_mqtt_connect_saves_credentials()
{
    FakeMqttClient mqtt;
    mqtt.connect("myClient", "testuser", "testpass");

    TEST_ASSERT_EQUAL_STRING("myClient", mqtt.receivedClientId.c_str());
    TEST_ASSERT_EQUAL_STRING("testuser", mqtt.receivedUsername.c_str());
    TEST_ASSERT_EQUAL_STRING("testpass", mqtt.receivedPassword.c_str());
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

// =============== dht (temp & humidity) ===============

// yeah I know I'm testing a test double so what?
void test_dht_sensor_returns_stubbed_data()
{
    DhtSensorFake sensor("temp_hum", "22.5,45");
    TEST_ASSERT_EQUAL_STRING("temp_hum", sensor.name().c_str());
    TEST_ASSERT_EQUAL_STRING("22.5,45", sensor.read().c_str());
}

// boot should work with any sensor
void test_boot_registers_and_publishes_dht_sensor()
{
    SensorRegistry registry;
    FakeMqttClient mqtt;

    std::vector<std::shared_ptr<ISensor>> sensors = {
        std::make_shared<DummySensor>("temp", "22.5"),
        std::make_shared<DummySensor>("humidity", "45")};

    bootSystem(sensors, registry, mqtt);

    TEST_ASSERT_EQUAL_STRING("esp32/available_sensors", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING("[\"temp\",\"humidity\"]", mqtt.lastMessage.c_str());

    auto names = registry.listNames();
    TEST_ASSERT_EQUAL(2, names.size());
    TEST_ASSERT_EQUAL_STRING("temp", names[0].c_str());
    TEST_ASSERT_EQUAL_STRING("humidity", names[1].c_str());
}

// boot should also register and publish real temperature and humidity sensors (even though they are one dht)
void test_boot_registers_and_publishes_real_sensors()
{
    SensorRegistry registry;
    FakeMqttClient mqtt;

    std::vector<std::shared_ptr<ISensor>> sensors = {
        std::make_shared<TemperatureSensor>(),
        std::make_shared<HumiditySensor>(),
    };

    bootSystem(sensors, registry, mqtt);

    TEST_ASSERT_EQUAL_STRING("esp32/available_sensors", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING("[\"temp\",\"humidity\"]", mqtt.lastMessage.c_str());
}

// =============== buzzer ===============
void test_buzzer_buzzes_on_request()
{
    SensorRegistry registry;
    FakeMqttClient mqtt;
    FakeBuzzer buzzer;
    dispatchMqttRequest("esp32/request/temp", registry, mqtt, buzzer);

    TEST_ASSERT_TRUE_MESSAGE(buzzer.buzzed, "Buzzer should buzz on any request");
}

// =============== camera ===============

void test_camera_driver_returns_base64_image()
{
    FakeCameraSensor camera;
    camera.init();

    std::string image = camera.read();

    // Basic sanity: must not be empty
    TEST_ASSERT_FALSE_MESSAGE(image.empty(), "Camera returned empty string");

    if (image.length() <= 10000)
    {
        Serial.print("Camera returned short output: ");
        Serial.println(image.c_str());
    }

    // check that it's at least bit
    TEST_ASSERT_TRUE_MESSAGE(image.length() > 10000, "Camera output too small to be a valid JPEG in base64");
}

void test_camera_read_fails_without_initialisation()
{
    FakeCameraSensor camera;
    std::string image = camera.read();

    TEST_ASSERT_EQUAL_STRING("error:not_initialized", image.c_str());
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
    RUN_TEST(test_mqtt_connect_saves_credentials);
    RUN_TEST(test_dht_sensor_returns_stubbed_data);
    RUN_TEST(test_boot_registers_and_publishes_dht_sensor);
    RUN_TEST(test_boot_registers_and_publishes_real_sensors);
    RUN_TEST(test_buzzer_buzzes_on_request);
    RUN_TEST(test_camera_driver_returns_base64_image);
    RUN_TEST(test_camera_read_fails_without_initialisation);

    return UNITY_END();
}