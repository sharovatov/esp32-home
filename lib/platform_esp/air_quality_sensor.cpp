#include "air_quality_sensor.h"
#include <Arduino.h>

MQ135Sensor::MQ135Sensor(int analogPin, int minRaw, int maxRaw)
    : AirQualitySensorBase(minRaw, maxRaw), pin(analogPin)
{
    pinMode(pin, INPUT);
}

std::string MQ135Sensor::name() const
{
    return "air_quality";
}

std::string MQ135Sensor::read()
{
    int raw = analogRead(pin);
    return std::to_string(calculatePercentage(raw));
}

std::string MQ135Sensor::type() const
{
    return "text/plain";
}