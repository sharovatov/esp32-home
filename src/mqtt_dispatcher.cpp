#include "mqtt_dispatcher.h"

void dispatchMqttRequest(const std::string &topic, const SensorRegistry &registry, MqttClient &mqtt)
{
    std::string prefix = "esp32/request/";
    if (topic.rfind(prefix, 0) != 0)
    {
        mqtt.publish("esp32/response/error", "invalid_topic:" + topic);
        return;
    }

    std::string sensorName = topic.substr(prefix.length());

    handleSensorRequest(sensorName, registry, mqtt);
}