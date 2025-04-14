#pragma once

#include "esp_err.h"

#include "i2c.h"

class CAP1188 {
public:
    CAP1188();
    ~CAP1188();
    esp_err_t initiate();

    esp_err_t get_input();

    esp_err_t action_verify();

private:
    I2c i2c_;
    i2c_master_dev_handle_t hDevice_;
    TaskHandle_t hTask_;

    esp_err_t setup_main_control();
    esp_err_t setup_sensitivity_control();
    esp_err_t setup_sensor_input_enable();
    esp_err_t setup_sensor_input_config();
    esp_err_t setup_avg_sample_config();
    esp_err_t setup_recal_config();

    static void vTask(void *pvParameters);
};

