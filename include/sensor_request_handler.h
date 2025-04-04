#pragma once

#include <string>
#include "sensor_registry.h"
#include "mqtt_client.h"

void handleSensorRequest(const std::string &sensorName, const SensorRegistry &registry, MqttClient &mqtt);