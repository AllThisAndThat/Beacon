#pragma once

#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#include "i2c.h"

/*
Add pause/active state
Paused - no measurement - low power mode

*/

class Ltr_303als {
public:
    Ltr_303als();
    ~Ltr_303als();
    esp_err_t initiate();

    uint32_t get_brightness();

    esp_err_t action_verify();

private:
    I2c i2c;
    i2c_master_dev_handle_t hDevice;
    TaskHandle_t hTask;
    volatile uint32_t brightness;

    static void vTask(void *pvParameters);
};