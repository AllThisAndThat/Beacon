#pragma once

#include "esp_err.h"
#include "driver/gpio.h"

enum class Logic_t: uint32_t
{
    LOW,
    HIGH
};


enum class PullState_t: uint32_t
{
    NO_PULL,
    PULL_UP,
    PULL_DOWN
};

enum class Enabled_t: uint32_t
{
    DISABLED,
    ENABLED,
};

enum class Configured_t: uint32_t
{
    NOT_CONFIGURED,
    CONFIGURED,
};

esp_err_t checkPin(gpio_num_t pin);
