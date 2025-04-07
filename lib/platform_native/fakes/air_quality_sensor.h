#pragma once
#include "sensor/isensor.h"
#include <string>
#include <cmath>
#include <algorithm>

class FakeAirQualitySensor : public ISensor
{
public:
    FakeAirQualitySensor(int rawMin, int rawMax)
        : rawMin(rawMin), rawMax(rawMax), rawValue(rawMin) {}

    void setRawValue(int value)
    {
        rawValue = value;
    }

    std::string name() const override
    {
        return "air_quality";
    }

    std::string read() override
    {
        float ratio = float(rawValue - rawMin) / float(rawMax - rawMin);
        int clamped = std::min(100, std::max(0, int(ratio * 100.0f + 0.5f)));
        return std::to_string(clamped);
    }

private:
    int rawMin;
    int rawMax;
    int rawValue;
};