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

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_registry_returns_available_sensor_names);

    return UNITY_END();
}