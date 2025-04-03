#pragma once

#include <string>
#include "sensor_registry.h"

class MqttClient
{
public:
    virtual void publish(const std::string &topic, const std::string &message) = 0;
    virtual ~MqttClient() = default;
};

void handleSensorRequest(const std::string &sensorName, const SensorRegistry &registry, MqttClient &mqtt);