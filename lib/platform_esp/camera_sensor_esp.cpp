#include "camera_sensor_esp.h"
#include "esp_camera.h"
#include "mbedtls/base64.h"
#include <esp_log.h>
#include <HardwareSerial.h>

RealCameraSensor::RealCameraSensor()
{
    // init() must be called separately
}

void RealCameraSensor::init()
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;

    // this cam uses 14 GPIO pins

    // pixel values sent through pin_d0 to pin_d7
    config.pin_d0 = 4;
    config.pin_d1 = 5;
    config.pin_d2 = 18;
    config.pin_d3 = 19;
    config.pin_d4 = 36;
    config.pin_d5 = 39;
    config.pin_d6 = 34;
    config.pin_d7 = 35;

    // input clock from the ESP32 to the camera
    config.pin_xclk = 21;

    // pixel clock from the camera to the ESP32
    config.pin_pclk = 22;

    config.pin_vsync = 25;
    config.pin_href = 23;

    // serial data line
    config.pin_sccb_sda = 26;

    // serial clock line
    config.pin_sccb_scl = 27;

    config.pin_pwdn = -1;
    config.pin_reset = -1;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_XGA;
    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode = CAMERA_GRAB_LATEST;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 2;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        ESP_LOGE("CameraSensor", "Camera init failed: 0x%x", err);
        return;
    }

    isInitialised = true;
}

std::string RealCameraSensor::read()
{
    if (!isInitialised)
    {
        Serial.println("[CameraSensor] Not initialised");
        return "error:not_initialized";
    }

    Serial.println("[CameraSensor] Attempting to capture frame...");

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb)
    {
        Serial.println("[CameraSensor] Failed to get frame buffer (fb is null)");
        return "error:no_frame";
    }

    Serial.printf("[CameraSensor] Frame captured: %u bytes\n", fb->len);

    size_t output_len = 4 * ((fb->len + 2) / 3) + 1;
    uint8_t *output_buffer = (uint8_t *)malloc(output_len);
    if (!output_buffer)
    {
        Serial.println("[CameraSensor] Failed to allocate memory for base64 encoding");
        esp_camera_fb_return(fb);
        return "error:malloc_failed";
    }
    Serial.printf("[CameraSensor] Allocated %u bytes for base64\n", output_len);

    size_t actual_len;
    int ret = mbedtls_base64_encode(output_buffer, output_len, &actual_len, fb->buf, fb->len);
    esp_camera_fb_return(fb);

    if (ret != 0)
    {
        Serial.printf("[CameraSensor] Base64 encoding failed with code %d\n", ret);

        free(output_buffer);
        return "error:base64_failed";
    }

    Serial.printf("[CameraSensor] Base64 encoded length: %u\n", actual_len);

    std::string result((char *)output_buffer, actual_len);
    free(output_buffer);

    Serial.println("[CameraSensor] Image read successfully");

    return result;
}

std::string RealCameraSensor::name() const
{
    return sensorName;
}