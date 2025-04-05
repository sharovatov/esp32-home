#include "sensor/sensor_publisher.h"

void publishAvailableSensors(const SensorRegistry &registry, MqttClient &mqtt)
{
    auto names = registry.listNames();

    std::string result = "[";
    for (size_t i = 0; i < names.size(); ++i)
    {
        result += "\"" + names[i] + "\"";
        if (i < names.size() - 1)
            result += ",";
    }
    result += "]";

    mqtt.publish("esp32/available_sensors", result);
}