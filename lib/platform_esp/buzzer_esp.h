#pragma once
#include "sensor/buzzer_sensor.h"

class Buzzer : public IBuzzer
{
public:
    explicit Buzzer(int pin);
    void buzz() override;

private:
    int pin;
};