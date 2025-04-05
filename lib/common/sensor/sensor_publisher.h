#pragma once

#include "sensor/sensor_registry.h"
#include "sensor/sensor_request_handler.h" // for MqttClient

void publishAvailableSensors(const SensorRegistry &registry, MqttClient &mqtt);