#pragma once
#include "sensor/isensor.h"
#include <string>

class DhtSensorFake : public ISensor
{
public:
    DhtSensorFake(const std::string &sensorName, const std::string &reading)
        : sensorName(sensorName), readingValue(reading) {}

    std::string name() const override { return sensorName; }

    std::string read() override { return readingValue; }

private:
    std::string sensorName;
    std::string readingValue;
};