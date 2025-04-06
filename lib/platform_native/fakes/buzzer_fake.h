#pragma once
#include "ibuzzer.h"

class FakeBuzzer : public IBuzzer
{
public:
    bool buzzed = false;

    void buzz() override
    {
        buzzed = true;
    }
};