#pragma once

#include <string>
#include "sensor/sensor_registry.h"
#include "mqtt/mqtt_client.h"

void handleSensorRequest(const std::string &sensorName, const SensorRegistry &registry, MqttClient &mqtt);