#pragma once
#include <vector>
#include <string>
#include <memory>
#include "isensor.h"

class SensorRegistry
{
private:
    std::vector<std::shared_ptr<ISensor>> sensors;

public:
    void add(std::shared_ptr<ISensor> sensor);
    std::vector<std::string> listNames() const;
    std::vector<std::shared_ptr<ISensor>> list() const;
};