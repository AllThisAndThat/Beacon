#pragma once

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

esp_err_t checkPin(gpio_num_t pin) 
{
    #ifdef ESP32S3_DEVKITC_1
        constexpr uint32_t KEY = 
            (((uint32_t)1)<<GPIO_NUM_19) |
            (((uint32_t)1)<<GPIO_NUM_20);
    #endif

    if (((uint32_t)1 << pin) && (KEY) != 0)
    {
        return ESP_ERR_INVALID_ARG;
    }
    else
    {
        return ESP_OK;
    }
}
