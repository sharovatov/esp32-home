# ESP32 Flat Monitor

A lightweight system for remotely checking on the flat — including temperature, humidity, and camera snapshots — using an ESP32, MQTT, and a simple web client.

## Overview

This project uses an ESP32 device equipped with:
- A camera ([OV5640](https://cdn.sparkfun.com/datasheets/Sensors/LightImaging/OV5640_datasheet.pdf))
- A temperature and humidity sensor ([DHT11](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf))
- A passive buzzer

The ESP32 connects to the home Wi-Fi and communicates via my own MQTT broker, providing access to real-time sensor data and user-triggered updates.

## MQTT Interface

### Discovery

ESP32 publishes a JSON list of currently available sensors to `esp32/available_sensors` topic.
  
Example payload:
  ```json
  [
    { "name": "temp", "type": "text/plain" },
    { "name": "humidity", "type": "text/plain" },
    { "name": "air_quality", "type": "text/plain" },
    { "name": "camera", "type": "image/jpeg" }
  ]
  ```

Clients subscribe to this topic on connect and use the metadata to dynamically render controls and correctly display responses.

### Requesting Sensor Data

The web client sends requests to `esp32/request/<sensorName>` topic.
  
The `<sensorName>` must match one of the available sensors.

#### Example:
```
esp32/request/camera      → triggers a new photo capture  
esp32/request/humidity    → triggers humidity reading  
esp32/request/air_quality → triggers air quality percentage
esp32/request/temp        → triggers temperature reading
```

ESP32 will buzz once to confirm reception of any valid or invalid request.

### Receiving Sensor Data

ESP32 publishes sensor results to `esp32/response/<sensorName>` topic.

- Topic pattern: esp32/response/<sensorName>
- Payload: Value returned by the sensor

#### Example payloads:
```
esp32/response/temp: "23.5"
esp32/response/humidity: "69.6"
esp32/response/air_quality: "47"
esp32/response/camera: (Base64-encoded JPEG string)
```

### Error Handling

Any malformed requests, unavailable sensors, or failures are published to `esp32/response/error` topic.

#### Example payload:
```
sensor_unknown:airflow
invalid_topic:bad/request/humidity
```

## 💻 Web Client

The frontend is a static HTML + JavaScript page that:
- Connects to the MQTT broker via WebSocket
- Subscribes to `esp32/available_sensors` and `esp32/response/#` topics
- Renders one button per available sensor using metadata from `available_sensors`
- Sends requests via MQTT on button click
- Displays each result dynamically based on the sensor type (text/plain or image/jpeg)

The client is fully decoupled from the ESP32 implementation. It relies exclusively on metadata provided via MQTT.

## Development & Testing

All ESP32 code is developed using Test-Driven Development (TDD) with separation of logic and hardware:

- Business logic is tested with PlatformIO’s `native` environment
-	ESP32-specific components are tested in esp32 integration tests
- All sensors are represented by test doubles (fakes/stubs) for isolated testing
- Sensor registry, MQTT dispatch, message structure, and system boot logic are tested without hardware

### ESP32 GPIO Usage Overview

| GPIO | Status       | Used By       | Purpose / Notes                                  |
|------|--------------|---------------|--------------------------------------------------|
| 0    | ⚠️ Avoid     | Boot pin      | Must be pulled LOW to enter flash mode           |
| 1    | ⚠️ UART TX   | Serial        | TX0 — used for programming/debug                 |
| 2    | ⚠️ Avoid     | Boot pin      | Must be LOW on boot; often used for onboard LED  |
| 3    | ⚠️ UART RX   | Serial        | RX0 — used for programming/debug                 |
| 4    | ❌ Occupied  | Camera        | D0                                               |
| 5    | ❌ Occupied  | Camera        | D1                                               |
| 12   | ⚠️ Caution   | -             | Must be LOW on boot — strapping pin              |
| 13   | ❌ Occupied  | ADC2          | Wifi uses ADC2, so no reading when Wifi=on       |
| 14   | ❌ Occupied  | Buzzer        | Passive buzzer signal                            |
| 15   | ✅ Free      | -             | Often used for SPI CS                            |
| 16   | ✅ Free      | -             | Can be used; UART2 RX                            |
| 17   | ✅ Free      | -             | Can be used; UART2 TX                            |
| 18   | ❌ Occupied  | Camera        | D2                                               |
| 19   | ❌ Occupied  | Camera        | D3                                               |
| 21   | ❌ Occupied  | Camera        | XCLK                                             |
| 22   | ❌ Occupied  | Camera        | PCLK                                             |
| 23   | ❌ Occupied  | Camera        | HREF                                             |
| 25   | ❌ Occupied  | Camera        | VSYNC                                            |
| 26   | ❌ Occupied  | Camera        | SCCB SDA                                         |
| 27   | ❌ Occupied  | Camera        | SCCB SCL                                         |
| 32   | ❌ Occupied  | MQ-135        | MQ-135 Air Quality sensor                        |
| 33   | ❌ Occupied  | DHT11         | Temp & humidity sensor                           |
| 34   | ❌ Occupied  | Camera        | D6 (input only)                                  |
| 35   | ❌ Occupied  | Camera        | D7 (input only)                                  |
| 36   | ❌ Occupied  | Camera        | D4 (input only)                                  |
| 39   | ❌ Occupied  | Camera        | D5 (input only)                                  |

### ✅ Legend:
- **✅ Free** – safe to use for sensors/peripherals  
- **❌ Occupied** – already used here
- **⚠️ Caution / Avoid** – avoid unless really sure