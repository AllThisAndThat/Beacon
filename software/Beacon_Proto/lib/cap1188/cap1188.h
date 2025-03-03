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
};

