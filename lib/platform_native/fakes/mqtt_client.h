#pragma once

#include "mqtt/mqtt_client.h"
#include <string>

class FakeMqttClient : public MqttClient
{
public:
    void publish(const std::string &topic, const std::string &message) override
    {
        lastTopic = topic;
        lastMessage = message;
    }

    void subscribe(const std::string &topic) override
    {
        // No-op for now
    }

    void setCallback(void (*callback)(char *, uint8_t *, unsigned int)) override
    {
        // No-op for now
    }

    void loop() override
    {
        // No-op for now
    }

    bool connect(const char *clientId) override
    {
        return true; // Assume always connected for test double
    }

    std::string lastTopic;
    std::string lastMessage;
};
