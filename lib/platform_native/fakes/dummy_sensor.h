class DummySensor : public ISensor
{
public:
    DummySensor(const std::string &sensorName, const std::string &reading, const std::string &sensorType = "text/plain")
        : sensorName(sensorName), readingValue(reading), sensorType(sensorType) {}

    std::string name() const override { return sensorName; }
    std::string read() override { return readingValue; }
    std::string type() const override { return sensorType; }

private:
    std::string sensorName;
    std::string readingValue;
    std::string sensorType;
};