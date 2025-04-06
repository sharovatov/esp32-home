#pragma once
#include "ibuzzer.h"

class Buzzer : public IBuzzer
{
public:
    explicit Buzzer(int pin);
    void buzz() override;

private:
    int pin;
};