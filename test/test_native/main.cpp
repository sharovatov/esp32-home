#include <unity.h>
#include <vector>
#include <string>
#include "sensor_registry.h"
#include "isensor.h"
#include "../../src/sensor_registry.cpp"

/*
  sensor test double
*/
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

// simplest MqttClient test double (mimicks mosquitto well enough)
class FakeMqttClient
{
public:
    void publish(const std::string &topic, const std::string &message)
    {
        lastTopic = topic;
        lastMessage = message;
    }

    std::string lastTopic;
    std::string lastMessage;
};

// publishes to esp32/available_sensors a string like ["camera","temp","humidity"]
void publishAvailableSensors(const SensorRegistry &registry, FakeMqttClient &mqtt)
{
    auto names = registry.listNames();

    std::string result = "[";
    for (size_t i = 0; i < names.size(); ++i)
    {
        result += "\"" + names[i] + "\"";
        if (i < names.size() - 1)
            result += ",";
    }
    result += "]";

    mqtt.publish("esp32/available_sensors", result);
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

// if the requested sensor is found on the registry, its readings should be published to mqtt
// if the sensor isn't found, corresponding error is published to mqtt
void handleSensorRequest(const std::string &sensorName, const SensorRegistry &registry, FakeMqttClient &mqtt)
{
    auto sensors = registry.list();

    for (const auto &sensor : sensors)
    {
        if (sensor->name() == sensorName)
        {
            std::string result = sensor->read();
            std::string topic = "esp32/response/" + sensorName;
            mqtt.publish(topic, result);
            return;
        }
    }

    std::string errorMessage = "sensor_unknown:" + sensorName;
    mqtt.publish("esp32/response/error", errorMessage);
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

// sensors should be able to respond to requests, starting off with camera
void test_sensor_request_is_routed_and_published()
{
    SensorRegistry registry;
    registry.add(std::make_shared<DummySensor>("camera", "imagebytes"));

    FakeMqttClient mqtt;

    handleSensorRequest("camera", registry, mqtt);

    TEST_ASSERT_EQUAL_STRING("esp32/response/camera", mqtt.lastTopic.c_str());
    TEST_ASSERT_EQUAL_STRING("imagebytes", mqtt.lastMessage.c_str());
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_registry_returns_available_sensor_names);
    RUN_TEST(test_publishes_available_sensor_names_to_mqtt);
    RUN_TEST(test_sensor_request_is_routed_and_published);
    RUN_TEST(test_unknown_sensor_publishes_error);

    return UNITY_END();
}