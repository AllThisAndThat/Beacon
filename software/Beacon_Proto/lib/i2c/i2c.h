#pragma once

#include "driver/i2c_master.h"
/*
Doesn't support faster than 400kHz

*/
enum class I2cPort {
    kPort0,
    kPort1
};

class I2c {
public:
    I2c(I2cPort port);
    ~I2c();
    esp_err_t initstate();

    esp_err_t action_add_device(i2c_device_config_t dev_cfg,
                                i2c_master_dev_handle_t& dev_handle);
    esp_err_t action_probe(uint16_t addr);

private:
    I2cPort port;
    i2c_master_bus_handle_t hBus;

    i2c_master_dev_handle_t hDevice_LTR_303;

};