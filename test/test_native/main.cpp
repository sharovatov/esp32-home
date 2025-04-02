#include <unity.h>
#include <vector>
#include <string>
#include "sensor_registry.h"
#include "../../src/sensor_registry.cpp"

/*
  I want the sensors registry to be populated with all the physically present sensors,
  so that later on, when I add another sensor to the board, I will just add another
  sensor to the registry, and also add supporting code (driver)
*/
void test_add_and_list_sensors()
{
    SensorRegistry registry;

    registry.add("camera");
    registry.add("temp");
    registry.add("humidity");

    auto sensors = registry.list();

    TEST_ASSERT_EQUAL(3, sensors.size());
    TEST_ASSERT_EQUAL_STRING("camera", sensors[0].c_str());
    TEST_ASSERT_EQUAL_STRING("temp", sensors[1].c_str());
    TEST_ASSERT_EQUAL_STRING("humidity", sensors[2].c_str());
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_add_and_list_sensors);

    return UNITY_END();
}