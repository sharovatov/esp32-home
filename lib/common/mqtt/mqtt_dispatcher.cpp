#include "mqtt/mqtt_dispatcher.h"

void dispatchMqttRequest(const std::string &topic, const SensorRegistry &registry, MqttClient &mqtt)
{
    std::string prefix = "esp32/request/";

    // check to see if the topic is correct
    if (topic.rfind(prefix, 0) != 0)
    {
        mqtt.publish("esp32/response/error", "invalid_topic:" + topic);
        return;
    }

    std::string sensorName = topic.substr(prefix.length());

    // check for empty sensor name
    if (sensorName.empty())
    {
        mqtt.publish("esp32/response/error", "sensor_unknown:");
        return;
    }

    handleSensorRequest(sensorName, registry, mqtt);
}