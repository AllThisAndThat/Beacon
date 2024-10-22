/*
Getting familar with c++. There will be lots of dumb comments.
*/

#pragma once // include only once in the compilation

// Standard includes
#include <stdint.h>

//esp idf includes
#include <driver/gpio.h>

//my library includes
#include "custom_gpio.h"


enum state_t 
{
    RUNNING,
    STOP
};

enum openClose_t 
{
    OPEN,
    CLOSED
};

enum error_t
{
    ERROR,
    NO_ERROR
};

class PWM
{
    public:
        // Constructor and Destructor
        PWM();
        ~PWM();

        // Open and Close
        openClose_t open();
        void close();


        // Getters
        uint32_t   getDuty() const;
        gpio_num_t getPin() const;
        logic_t    getDefaultLogic() const;
        state_t    getState() const;

        // Setters
        error_t setAll(uint32_t duty, gpio_num_t pin, logic_t defaultLogic);
        error_t setDuty(uint32_t duty);
        void    setPin(gpio_num_t pin);

        // Write (and Read)
        error_t writeState(state_t state);

    private:
        static openClose_t driverProtection;

        uint32_t duty;
        gpio_num_t pin;
        logic_t defaultLogic;
        openClose_t instanceAccess;
        state_t state;

        error_t checkDuty(uint32_t duty);

        void gpio_setup();
};

