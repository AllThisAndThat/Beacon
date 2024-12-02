#pragma once

#include <stdio.h>
#include "digital_output_object.h"


class PinManager 
{
    public:
        static PinManager* Instance();

        //can write an enable for each type individually or find clever way to use one function
        esp_err_t enableDigitalOutputObject(DigitalOutputObject* object);
        esp_err_t disableDigitalOutputObject(DigitalOutputObject* object);

    protected:
        PinManager();

    private:
        static PinManager *mInstance;

        static uint32_t myBitSet;
};
