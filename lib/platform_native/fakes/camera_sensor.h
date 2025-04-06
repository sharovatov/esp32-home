#pragma once
#include "sensor/isensor.h"
#include <string>

class FakeCameraSensor : public ISensor
{
public:
    FakeCameraSensor(const std::string &name = "camera") : sensorName(name) {}

    void init()
    {
        initialised = true;
    }

    std::string name() const override
    {
        return sensorName;
    }

    std::string read() override
    {
        if (!initialised)
            return "error:not_initialized";

        // stub base64-encoded quasi JPEG file: \xFF\xD8 ... \xFF\xD9
        // decodes to a very short binary JPEG-like structure
        return "/9j/4AAQSkZJRgABAQAAAQABAAD/2wCEAA==";
    }

private:
    std::string sensorName;
    bool initialised = false;
};