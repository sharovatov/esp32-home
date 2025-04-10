#pragma once
#include "sensor/air_quality_sensor_base.h"
#include <string>

class MQ135Sensor : public AirQualitySensorBase
{
public:
    MQ135Sensor(int analogPin, int minRaw, int maxRaw);

    std::string name() const override;
    std::string read() override;
    std::string type() const override;

private:
    int pin;
};