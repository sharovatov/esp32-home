#include "sensor_registry.h"

void SensorRegistry::add(std::shared_ptr<ISensor> sensor)
{
    sensors.push_back(sensor);
}

std::vector<std::string> SensorRegistry::listNames() const
{
    std::vector<std::string> names;
    for (const auto &s : sensors)
    {
        names.push_back(s->name());
    }
    return names;
}
