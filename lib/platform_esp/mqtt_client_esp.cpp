#include "mqtt_client_esp.h"

RealMqttClient::RealMqttClient(Client &wifiClient, const char *host, uint16_t port)
    : client(wifiClient)
{
    client.setServer(host, port);
}

bool RealMqttClient::connect(const char *clientId, const char *username, const char *password)
{
    return client.connect(clientId, username, password);
}

void RealMqttClient::publish(const std::string &topic, const std::string &message)
{
    client.publish(topic.c_str(), message.c_str());
}

void RealMqttClient::publish(const std::string &topic, const std::string &message, bool retain)
{
    client.publish(topic.c_str(), message.c_str(), retain);
}

void RealMqttClient::subscribe(const std::string &topic)
{
    client.subscribe(topic.c_str());
}

void RealMqttClient::setCallback(void (*callback)(char *, uint8_t *, unsigned int))
{
    client.setCallback(callback);
}

void RealMqttClient::loop()
{
    client.loop();
}