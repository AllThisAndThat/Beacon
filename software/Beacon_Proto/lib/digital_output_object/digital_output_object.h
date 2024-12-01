#pragma once

#include <stdio.h>

#include "esp_err.h"
#include "driver/gpio.h"

#include "shared_variables.h"


enum class OutputMode_t: uint32_t
{
    PUSH_PULL,
    OPEN_DRAIN
};


typedef struct
{
    gpio_num_t pin;
    OutputMode_t mode;
    PullState_t pullState;
    //drive strength
    //input/output
} DigitalOutputConfig;


/*
For now, only supports one pin at a time.
Only support for esp_error at this time
*/
class DigitalOutputObject
{
    public:
        DigitalOutputObject();
        ~DigitalOutputObject();

        esp_err_t setConfig(DigitalOutputConfig config);
        esp_err_t configure(); 
        esp_err_t set_action(Logic_t logic);
        esp_err_t set_toggle();

        DigitalOutputConfig getConfig() const;
        bool getIfIsEnabled() const;
        bool getIfIsReady() const;

        friend class PinManager;

    private:
        DigitalOutputConfig config;
        Logic_t logic;
        bool isEnabled;
        bool isReady; 
};







