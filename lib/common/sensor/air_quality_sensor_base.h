#pragma once
#include "sensor/isensor.h"
#include <string>
#include <cmath>
#include <algorithm>

class AirQualitySensorBase : public ISensor
{
public:
    AirQualitySensorBase(int minRaw, int maxRaw)
        : minRaw(minRaw), maxRaw(maxRaw) {}

protected:
    int calculatePercentage(int raw) const
    {
        int clamped = std::min(std::max(raw, minRaw), maxRaw);
        float ratio = 1.0f - static_cast<float>(clamped - minRaw) / (maxRaw - minRaw);
        return std::round(ratio * 100);
    }

private:
    int minRaw;
    int maxRaw;
};