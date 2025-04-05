#include "boot/boot.h"
#include "sensor/sensor_publisher.h"

void bootSystem(const std::vector<std::shared_ptr<ISensor>> &sensors,
                SensorRegistry &registry,
                MqttClient &mqtt)
{
    for (const auto &sensor : sensors)
    {
        registry.add(sensor);
    }

    publishAvailableSensors(registry, mqtt);
}