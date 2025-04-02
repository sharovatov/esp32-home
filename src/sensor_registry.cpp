#include "sensor_registry.h"

std::vector<std::string> SensorRegistry::list() const
{
    return {"camera", "temp", "humidity"};
}