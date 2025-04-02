#include "sensor_registry.h"

std::vector<std::string> SensorRegistry::list() const
{
    return sensors;
}

void SensorRegistry::add(const std::string &sensor)
{
    sensors.push_back(sensor);
}
