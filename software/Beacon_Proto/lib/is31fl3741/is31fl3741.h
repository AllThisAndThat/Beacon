#pragma once

#include "esp_err.h"

#include "i2c.h"

enum class Page: uint8_t {
    // Page 9
    k0 = 0b0000'0000,
    k1 = 0b0000'0001,
    k2 = 0b0000'0010,
    k3 = 0b0000'0011,
    k4 = 0b0000'0100
};

class IS31FL3741 {
public:
    IS31FL3741();
    ~IS31FL3741();
    esp_err_t initiate();

    esp_err_t set_global_current(uint8_t current);

    esp_err_t action_verify();
    esp_err_t action_off();
    esp_err_t action_on();

private:
    I2c i2c_;
    i2c_master_dev_handle_t hDevice_;

    esp_err_t setup_config_register();
    esp_err_t setup_pull_resistor();

    esp_err_t action_unlock(Page page);
};
