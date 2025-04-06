#include "temperature_sensor_esp.h"

TemperatureSensor::TemperatureSensor(uint8_t pin, uint8_t type, const std::string &sensorName)
    : sensorName(sensorName), dht(pin, type)
{
    dht.begin();
}

std::string TemperatureSensor::name() const
{
    return sensorName;
}

std::string TemperatureSensor::read()
{
    float t = dht.readTemperature();
    if (isnan(t))
    {
        return "error";
    }

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%.1f", t);
    return std::string(buffer);
}
