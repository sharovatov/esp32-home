#include <unity.h>
#include <vector>
#include <string>
#include "sensor_registry.h"

/*
  When asked, the system returns a list of available sensors.
  Currently the system supports three sensors: camera, temp, humidity
*/
void test_returns_available_sensors()
{
    SensorRegistry registry;
    auto sensors = registry.list();

    // hardcoding the values for now
    TEST_ASSERT_EQUAL(3, sensors.size());
    TEST_ASSERT_EQUAL_STRING("camera", sensors[0].c_str());
    TEST_ASSERT_EQUAL_STRING("temp", sensors[1].c_str());
    TEST_ASSERT_EQUAL_STRING("humidity", sensors[2].c_str());
}
