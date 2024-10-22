#pragma once

#include <stdint.h>

#include <esp_err.h>
#include <driver/gpio.h>


enum logic_t: uint32_t
{
    LOW,
    HIGH
};


esp_err_t gpio_toggle_level(gpio_num_t pin);