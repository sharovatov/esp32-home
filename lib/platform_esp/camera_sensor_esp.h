#pragma once
#include "sensor/camera_sensor.h"
#include "esp_camera.h"
#include <string>

class RealCameraSensor : public CameraSensor
{
public:
    RealCameraSensor(); // now has no parameters
    void init();
    std::string name() const override;
    std::string read() override;
    std::string type() const override;

private:
    const std::string sensorName = "camera";
    bool isInitialised = false;
};