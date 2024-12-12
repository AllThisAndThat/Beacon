#pragma once

#include "driver/ledc.h"

/*
Functionality to add:
channel find or maybe the user specifies which channel (useful for RGB where a shared timer would be nice)
Change timer
Change channel
Change GPIO
Fading
On/Off
Duty as (0 > 1000)
Might keep track of which timers and channels in use
write test file
*/

enum class Logic : unsigned int
{
    kLow = 0,
    kHigh = 1
};

class LedDriver
{
    public:
        LedDriver();
        ~LedDriver(); 
        esp_err_t initiate(gpio_num_t gpio_num, ledc_timer_t timer,
            ledc_channel_t channel, Logic logic = Logic::kLow);

        esp_err_t set_duty(uint32_t duty);
        esp_err_t action_off();
        esp_err_t action_on();

    private:
        ledc_timer_t timer;
        ledc_channel_t channel;
        gpio_num_t pin;
        uint32_t duty;

        esp_err_t configure_init_timer(ledc_timer_t timer);
        esp_err_t configure_init_channel(gpio_num_t gpio_num, 
            ledc_channel_t channel, Logic logic);
};