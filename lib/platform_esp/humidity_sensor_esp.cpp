#include "humidity_sensor_esp.h"

HumiditySensor::HumiditySensor(uint8_t pin, uint8_t type, const std::string &sensorName)
    : sensorName(sensorName), dht(pin, type)
{
    dht.begin();
}

std::string HumiditySensor::name() const
{
    return sensorName;
}

std::string HumiditySensor::read()
{
    float h = dht.readHumidity();
    if (isnan(h))
    {
        return "error";
    }
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%.1f", h);
    return std::string(buffer);
}