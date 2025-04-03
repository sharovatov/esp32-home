#pragma once

#include <string>
#include "sensor_registry.h"
#include "sensor_request_handler.h" // for MqttClient

void dispatchMqttRequest(const std::string &topic, const SensorRegistry &registry, MqttClient &mqtt);