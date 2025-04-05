#pragma once
#include "sensor/isensor.h"
#include <string>

class HumiditySensor : public ISensor
{
public:
    std::string name() const override
    {
        return "humidity";
    }

    std::string read() override
    {
        return "45"; // stub for now
    }
};