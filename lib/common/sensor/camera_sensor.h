#pragma once
#include "sensor/isensor.h"
#include <string>

class CameraSensor : public ISensor
{
public:
    virtual ~CameraSensor() = default;
    virtual std::string read() override = 0;
};