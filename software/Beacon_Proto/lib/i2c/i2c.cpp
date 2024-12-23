#include "i2c.h"

#include "reserved_objects.h"

I2c::I2c(I2cPort port) {
    this->port = port;
}

I2c::~I2c() {
    // i2c_master_bus_rm_device();
}

esp_err_t I2c::initstate() {
    if (this->port == I2cPort::kPort0) {
        return i2c_new_master_bus(&reserved_i2c0::kBusCfg, &hBus);
    }
    else {
        // return i2c_new_master_bus(&reserved_i2c1::kBusCfg, &hBus);)
    }
    return ESP_OK;
}

esp_err_t I2c::action_add_device(i2c_device_config_t dev_cfg,
                                 i2c_master_dev_handle_t& dev_handle) {
    return i2c_master_bus_add_device(this->hBus, &dev_cfg, &dev_handle);
}

esp_err_t I2c::action_probe(uint16_t addr) {
    constexpr int kTimeoutMilliSeconds = 10;
    return i2c_master_probe(this->hBus, addr, kTimeoutMilliSeconds);
}
