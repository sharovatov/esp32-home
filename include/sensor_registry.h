#pragma once
#include <vector>
#include <string>

class SensorRegistry
{
public:
    std::vector<std::string> list() const;
};