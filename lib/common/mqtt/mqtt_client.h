#pragma once
#include <string>

class MqttClient
{
public:
    virtual bool connect(const char *clientId) = 0;
    virtual void publish(const std::string &topic, const std::string &message) = 0;
    virtual void subscribe(const std::string &topic) = 0;
    virtual void setCallback(void (*callback)(char *, uint8_t *, unsigned int)) = 0;
    virtual void loop() = 0;
    virtual ~MqttClient() = default;
};