#pragma once

#include "driver/spi_master.h"
#include "esp_err.h"

#include "spi.h"


class BMI323 {
public:
    BMI323();
    ~BMI323();
    esp_err_t initiate();

    esp_err_t action_verify();

private:
    Spi spi_;

    esp_err_t set_spi_mode();
};