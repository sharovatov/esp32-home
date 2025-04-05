#pragma once

#include "sensor/isensor.h"
#include <DHT.h>
#include <string>

class DhtSensor : public ISensor
{
public:
    DhtSensor(uint8_t pin, uint8_t type, const std::string &sensorName);

    std::string name() const override;
    std::string read() override;

private:
    DHT dht;
    std::string sensorName;
};