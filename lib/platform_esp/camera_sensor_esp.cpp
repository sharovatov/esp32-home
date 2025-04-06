#include "camera_sensor_esp.h"
#include "esp_camera.h"
#include "mbedtls/base64.h"
#include <esp_log.h>

RealCameraSensor::RealCameraSensor()
{
    // init() must be called separately
}

void RealCameraSensor::init()
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = 4;
    config.pin_d1 = 5;
    config.pin_d2 = 18;
    config.pin_d3 = 19;
    config.pin_d4 = 36;
    config.pin_d5 = 39;
    config.pin_d6 = 34;
    config.pin_d7 = 35;
    config.pin_xclk = 21;
    config.pin_pclk = 22;
    config.pin_vsync = 25;
    config.pin_href = 23;
    config.pin_sccb_sda = 26;
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
        return "error:not_initialized";

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb)
        return "error:no_frame";

    size_t output_len = 4 * ((fb->len + 2) / 3) + 1;
    uint8_t *output_buffer = (uint8_t *)malloc(output_len);
    if (!output_buffer)
    {
        esp_camera_fb_return(fb);
        return "error:malloc_failed";
    }

    size_t actual_len;
    int ret = mbedtls_base64_encode(output_buffer, output_len, &actual_len, fb->buf, fb->len);
    esp_camera_fb_return(fb);

    if (ret != 0)
    {
        free(output_buffer);
        return "error:base64_failed";
    }

    std::string result((char *)output_buffer, actual_len);
    free(output_buffer);
    return result;
}

std::string RealCameraSensor::name() const
{
    return sensorName;
}