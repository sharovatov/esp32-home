# ESP32 Flat Monitor

**Goal:**  
A lightweight system for remotely checking on the flat — including temperature, humidity, and camera snapshots — using an ESP32, MQTT, and a simple web client.

---

## 🌐 Overview

This project uses an ESP32 device equipped with:
- A camera ([OV5640](https://cdn.sparkfun.com/datasheets/Sensors/LightImaging/OV5640_datasheet.pdf))
- A temperature and humidity sensor ([DHT11](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf))

The ESP32 connects to the home Wi-Fi and communicates via my own MQTT broker, providing access to real-time sensor data and user-triggered updates.

---

## 🧩 MQTT Interface

### 📰 Discovery

- `esp32/available_sensors`  
  The ESP32 publishes a JSON list of currently available sensors.  
  Example payload:
  ```json
  ["humidity", "temperature", "camera"]
  ```

---

### Requesting Sensor Data

- `esp32/request/<sensorName>`  
  The web client sends requests here.  
  The `<sensorName>` must match one of the available sensors.

  #### Example:
  ```
  esp32/request/camera      → triggers a new photo capture  
  esp32/request/humidity    → triggers humidity reading  
  esp32/request/temperature → triggers temperature reading
  ```

---

### Receiving Sensor Data

- `esp32/response/<sensorName>`  
  ESP32 publishes sensor results to this topic.

  #### Example payloads:

  - **Camera (`esp32/response/camera`)**:  
    Base64-encoded image string  
    *(The receiving client is expected to render it as an image)*

  - **Humidity**: `69.60`

  - **Temperature**: `23.5`

---

### Error Handling

- `esp32/response/error`  
  Any malformed requests, unavailable sensors, or failures are published here.

  #### Example payload: `Unknown sensor: sound`

---

## 💻 Web Client

The frontend is a static HTML + JavaScript page that:
- Connects to the same MQTT broker via WebSocket
- Subscribes to `esp32/available_sensors` and all `esp32/response/#` topics
- Displays each sensor and a "Refresh" button
- Sends sensor requests via MQTT on button click
- Renders live sensor values or error messages

---

## 🧪 Development & Testing

The core logic of the system is developed using **Test-Driven Development (TDD)**:

- Logic is written and tested locally using PlatformIO’s `native` environment
- Tests use the Unity test framework
- Hardware and integration behaviour is added incrementally once core logic is verified

Sensor handling, message formatting, encoding, and MQTT dispatching are all tested independently of hardware.
