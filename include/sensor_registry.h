#pragma once
#include <vector>
#include <string>

class SensorRegistry
{
public:
    std::vector<std::string> list() const;
    void add(const std::string &sensor);

private:
    std::vector<std::string> sensors;
};
