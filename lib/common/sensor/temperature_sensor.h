#pragma once
#include "sensor/isensor.h"
#include <string>

class TemperatureSensor : public ISensor
{
public:
    std::string name() const override
    {
        return "temp";
    }

    std::string read() override
    {
        return "22.5"; // stub for now
    }

    std::string type() const override
    {
        return "text/plain";
    }
};