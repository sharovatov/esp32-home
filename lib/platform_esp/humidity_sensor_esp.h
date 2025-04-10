#pragma once
#include "sensor/isensor.h"
#include <DHT.h>

class HumiditySensor : public ISensor
{
public:
    HumiditySensor(uint8_t pin, uint8_t type);
    std::string name() const override;
    std::string read() override;
    std::string type() const override;

private:
    std::string sensorName;
    mutable DHT dht;
};