#include "i2c.h"

#include "reserved_objects.h"

constexpr int kTimeoutMilliSeconds = 1000;

I2c::I2c(I2cPort port) {
    this->port = port;
}

I2c::~I2c() {
    // i2c_master_bus_rm_device();
}

esp_err_t I2c::initiate() {
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
    return i2c_master_bus_add_device(this->hBus, &dev_cfg,
                                     &dev_handle);
}

esp_err_t I2c::action_probe(uint16_t addr) {
    
    return i2c_master_probe(this->hBus, addr, kTimeoutMilliSeconds);
}

esp_err_t I2c::action_write(i2c_master_dev_handle_t dev_handle, 
                            uint8_t register_addr, uint8_t data) {
    
    uint8_t temp[2] = {register_addr, data};
    constexpr size_t kSize = sizeof(temp);
    return i2c_master_transmit(dev_handle, temp, kSize,
                               kTimeoutMilliSeconds);
}

esp_err_t I2c::action_read(i2c_master_dev_handle_t dev_handle, 
                           uint8_t register_addr, uint8_t* data) {
    constexpr size_t kByteSize = sizeof(uint8_t);
    return i2c_master_transmit_receive(dev_handle, &register_addr,
                                       kByteSize, data, kByteSize,
                                       kTimeoutMilliSeconds);
}