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
    esp_err_t initiate();

    esp_err_t action_add_device(i2c_device_config_t dev_cfg,
                                i2c_master_dev_handle_t& dev_handle);
    esp_err_t action_probe(uint16_t addr);
    esp_err_t action_write_reg(i2c_master_dev_handle_t dev_handle, 
                            uint8_t register_addr, uint8_t data);
    esp_err_t action_write_direct(i2c_master_dev_handle_t dev_handle,
                                  uint8_t data);
    esp_err_t action_read(i2c_master_dev_handle_t dev_handle, 
                           uint8_t register_addr, uint8_t* data);

private:
    I2cPort port;
    i2c_master_bus_handle_t hBus_;


};