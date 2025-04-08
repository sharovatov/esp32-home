#pragma once

class IBuzzer
{
public:
    virtual void buzz() = 0;
    virtual ~IBuzzer() = default;
};