#pragma once

#include "esp_err.h"

#include "i2c.h"


class Max9867 {
public:
    Max9867();
    ~Max9867();
    esp_err_t initialize();

    esp_err_t set_volume(uint8_t volume);
    esp_err_t set_mute(bool mute);
    esp_err_t set_power(bool power);
    esp_err_t set_input(bool input);
    esp_err_t set_bypass(bool bypass);

private:
    I2c i2c;
    i2c_port_t hDevice;
    TaskHandle_t hTask;
    uint8_t volume;
    bool mute;
    bool power;
    bool input;
    bool bypass;
    void vTask(void *pvParameters);
};