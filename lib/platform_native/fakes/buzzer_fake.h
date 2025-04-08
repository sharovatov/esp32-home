#pragma once
#include "sensor/buzzer_sensor.h"

class FakeBuzzer : public IBuzzer
{
public:
    bool buzzed = false;

    void buzz() override
    {
        buzzed = true;
    }
};