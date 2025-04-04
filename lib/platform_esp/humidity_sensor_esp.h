#pragma once
#include "sensor/isensor.h"
#include <DHT.h>

class HumiditySensor : public ISensor
{
public:
    HumiditySensor(uint8_t pin, uint8_t type, const std::string &sensorName);
    std::string name() const override;
    std::string read() override;

private:
    std::string sensorName;
    mutable DHT dht;
};