#pragma once

#include "driver/ledc.h"

/*
Functionality to add
channel find or maybe the user specifies which channel (useful for RGB where a shared timer would be nice)
Change timer
Change channel
Change GPIO
Fading
Inverted default output (set under ledc_channel_config_t)
On/Off
Duty as (0 > 1000)
*/


class LedDriver
{
    public:
        LedDriver();
        ~LedDriver();
        esp_err_t initiate(gpio_num_t gpio_num);

        esp_err_t set_duty(uint32_t duty);

    private:
        static uint32_t reserved_timer_mask; // 4 timers

        ledc_timer_t timer;
        bool isInitiated;

        gpio_num_t pin;
        uint32_t duty;

        esp_err_t configure_init_timer();
        esp_err_t configure_init_channel(gpio_num_t gpio_num) const;

        ledc_timer_t find_timer();
};