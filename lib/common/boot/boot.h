#pragma once

#include <vector>
#include <memory>
#include <string>
#include "sensor/isensor.h"
#include "sensor/sensor_registry.h"
#include "sensor/sensor_request_handler.h"

void bootSystem(const std::vector<std::shared_ptr<ISensor>> &sensors,
                SensorRegistry &registry,
                MqttClient &mqtt);