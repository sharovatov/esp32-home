#pragma once

#include "sensor_registry.h"
#include "sensor_request_handler.h" // for MqttClient

void publishAvailableSensors(const SensorRegistry &registry, MqttClient &mqtt);