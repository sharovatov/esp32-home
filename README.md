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

### Usage

Instructions on how to build, upload and monitor will go here.

### 🧪 JavaScript Client Generation

This project auto-generates a `sensors.js` file that lists all available sensors for use in your web client.

#### 🔧 How it works

- A PlatformIO [extra script](tools/generate_sensors_js.py) runs during each build and generates a file named `sensors.js`.
- This file exports a list of available sensor names, e.g.:

```js
// Auto-generated file

export const availableSensors = [
  "camera",
  "temp",
  "humidity",
];
```

#### 📄 Output file

- The generated file will be saved at the root of the project as `sensors.js`
- You can include this in your static site to drive a client interface.

#### 🧪 Testing the generator

The JS generation logic is tested using standard Python unit tests.

To run the tests:

```bash
python3 -m unittest test/test_generate_sensors_js.py
```

This test validates the structure and output of the generated file.

### 🧭 ESP32 GPIO Usage Overview

| GPIO | Status       | Used By       | Purpose / Notes                                  |
|------|--------------|---------------|--------------------------------------------------|
| 0    | ⚠️ Avoid     | Boot pin      | Must be pulled LOW to enter flash mode           |
| 1    | ⚠️ UART TX   | Serial        | TX0 — used for programming/debug                 |
| 2    | ⚠️ Avoid     | Boot pin      | Must be LOW on boot; often used for onboard LED  |
| 3    | ⚠️ UART RX   | Serial        | RX0 — used for programming/debug                 |
| 4    | ❌ Occupied  | Camera        | D0                                               |
| 5    | ❌ Occupied  | Camera        | D1                                               |
| 12   | ⚠️ Caution   | -             | Must be LOW on boot — strapping pin              |
| 13   | ✅ Free      | -             | Safe, often used for SPI                         |
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
| 32   | ✅ Free      | -             | Safe; ADC1 input                                 |
| 33   | ❌ Occupied  | DHT11         | Temp & humidity sensor                           |
| 34   | ❌ Occupied  | Camera        | D6 (input only)                                  |
| 35   | ❌ Occupied  | Camera        | D7 (input only)                                  |
| 36   | ❌ Occupied  | Camera        | D4 (input only)                                  |
| 39   | ❌ Occupied  | Camera        | D5 (input only)                                  |

### ✅ Legend:
- **✅ Free** – safe to use for sensors/peripherals  
- **❌ Occupied** – already used here
- **⚠️ Caution / Avoid** – avoid unless really sure