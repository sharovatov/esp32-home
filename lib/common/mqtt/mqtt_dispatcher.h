#pragma once

#include <string>
#include "sensor/buzzer_sensor.h"
#include "sensor/sensor_registry.h"
#include "sensor/sensor_request_handler.h" // for MqttClient

void dispatchMqttRequest(const std::string &topic, const SensorRegistry &registry, MqttClient &mqtt, IBuzzer &buzzer);