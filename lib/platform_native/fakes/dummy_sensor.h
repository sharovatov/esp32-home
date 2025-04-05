class DummySensor : public ISensor
{
public:
    DummySensor(std::string sensorName, std::string result)
        : sensorName(sensorName), result(result) {}

    std::string name() const override { return sensorName; }
    std::string read() override { return result; }

private:
    std::string sensorName;
    std::string result;
};