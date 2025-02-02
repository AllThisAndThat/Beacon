#pragma once

#include "driver/spi_master.h"
#include "esp_err.h"

#include "spi.h"

/* 
Multi-read
*/

class BMI323 {
public:
    BMI323();
    ~BMI323();
    esp_err_t initiate();

    esp_err_t get_accelerometer_data(int16_t acc_data[3]) const;
    esp_err_t get_gyro_data(int16_t acc_data[3]) const;

    esp_err_t action_verify();
private:
    Spi spi_;

    esp_err_t setup_accelerometer() const;
    esp_err_t setup_gyro() const;

    esp_err_t action_write(const uint8_t addr,
                           const uint8_t data[2]) const;
    esp_err_t action_read_single(const uint8_t addr,
                                 uint8_t data[2]) const;
    esp_err_t action_read_multi(const uint8_t start_addr,
                                uint8_t* data, size_t length) const;
};