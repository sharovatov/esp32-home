#pragma once
#include "sensor/air_quality_sensor_base.h"

class FakeAirQualitySensor : public AirQualitySensorBase
{
public:
    FakeAirQualitySensor(int minRaw, int maxRaw)
        : AirQualitySensorBase(minRaw, maxRaw), currentRaw(minRaw) {}

    void setRawValue(int value)
    {
        currentRaw = value;
    }

    std::string name() const override
    {
        return "air_quality";
    }

    std::string read() override
    {
        return std::to_string(calculatePercentage(currentRaw));
    }

private:
    int currentRaw;
};