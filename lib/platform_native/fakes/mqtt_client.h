#pragma once
#include "mqtt/mqtt_client.h"
#include <string>

class FakeMqttClient : public MqttClient
{
public:
    std::string lastTopic;
    std::string lastMessage;
    std::string subscribedTopic;
    std::string receivedClientId;
    std::string receivedUsername;
    std::string receivedPassword;
    void (*callback)(char *, uint8_t *, unsigned int) = nullptr;

    bool connect(const char *clientId, const char *username, const char *password) override
    {
        receivedClientId = clientId;
        receivedUsername = username;
        receivedPassword = password;
        return true;
    }

    void publish(const std::string &topic, const std::string &message) override
    {
        lastTopic = topic;
        lastMessage = message;
    }

    void subscribe(const std::string &topic) override
    {
        subscribedTopic = topic;
    }

    void setCallback(void (*cb)(char *, uint8_t *, unsigned int)) override
    {
        callback = cb;
    }

    void loop() override
    {
        // optional: simulate MQTT loop behaviour
    }
};