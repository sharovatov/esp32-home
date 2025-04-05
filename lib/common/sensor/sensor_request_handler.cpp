#include "sensor/sensor_request_handler.h"

void handleSensorRequest(const std::string &sensorName, const SensorRegistry &registry, MqttClient &mqtt)
{
    auto sensors = registry.list();

    for (const auto &sensor : sensors)
    {
        if (sensor->name() == sensorName)
        {
            std::string result = sensor->read();
            std::string topic = "esp32/response/" + sensorName;
            mqtt.publish(topic, result);
            return;
        }
    }

    std::string errorMessage = "sensor_unknown:" + sensorName;
    mqtt.publish("esp32/response/error", errorMessage);
}
