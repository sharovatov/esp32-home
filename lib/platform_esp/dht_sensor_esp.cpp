#include "dht_sensor_esp.h"

DhtSensor::DhtSensor(uint8_t pin, uint8_t type, const std::string &sensorName)
    : dht(pin, type), sensorName(sensorName)
{
    dht.begin();
}

std::string DhtSensor::name() const
{
    return sensorName;
}

std::string DhtSensor::read()
{
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t))
    {
        return "error";
    }

    return std::to_string(t) + "," + std::to_string(h);
}