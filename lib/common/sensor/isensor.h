#pragma once
#include <string>

class ISensor
{
public:
    virtual ~ISensor() = default;
    virtual std::string name() const = 0;
    virtual std::string read() = 0;
    virtual std::string type() const = 0;
};