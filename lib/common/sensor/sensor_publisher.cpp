#include "sensor/sensor_publisher.h"

void publishAvailableSensors(const SensorRegistry &registry, MqttClient &mqtt)
{
    auto sensors = registry.list();

    std::string result = "[";
    for (size_t i = 0; i < sensors.size(); ++i)
    {
        const auto &s = sensors[i];
        result += "{\"name\":\"" + s->name() + "\",\"type\":\"" + s->type() + "\"}";
        if (i < sensors.size() - 1)
            result += ",";
    }
    result += "]";

    mqtt.publish("esp32/available_sensors", result, true);
}