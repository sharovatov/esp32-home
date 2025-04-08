#pragma once

#include "mqtt/mqtt_client.h"
#include <PubSubClient.h>
#include <WiFi.h>

class RealMqttClient : public MqttClient
{
public:
    RealMqttClient(Client &wifiClient, const char *host, uint16_t port);

    bool connect(const char *clientId, const char *username, const char *password) override;
    void publish(const std::string &topic, const std::string &message) override;
    void publish(const std::string &topic, const std::string &message, bool retain) override;
    void subscribe(const std::string &topic) override;
    void setCallback(void (*callback)(char *, uint8_t *, unsigned int)) override;
    void loop() override;

private:
    PubSubClient client;
};