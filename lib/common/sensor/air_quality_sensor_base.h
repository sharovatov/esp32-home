#pragma once
#include "sensor/isensor.h"
#include <string>
#include <cmath>
#include <algorithm>
#ifdef ARDUINO
#include <HardwareSerial.h>
#endif

class AirQualitySensorBase : public ISensor
{
public:
    AirQualitySensorBase(int minRaw, int maxRaw)
        : minRaw(minRaw), maxRaw(maxRaw) {}

protected:
    int calculatePercentage(int raw) const
    {
#ifdef ARDUINO
        Serial.printf("[MQ135Sensor] std::min(std::max(%d, %d), %d)\n", raw, minRaw, maxRaw);
#endif
        int clamped = std::min(std::max(raw, minRaw), maxRaw);
        float ratio = 1.0f - static_cast<float>(clamped - minRaw) / (maxRaw - minRaw);
        return std::round(ratio * 100);
    }

private:
    int minRaw;
    int maxRaw;
};