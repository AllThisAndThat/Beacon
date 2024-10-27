
#pragma once

#include <stdint.h>
// #include <bitset>

#include <driver/gpio.h>

#include "enum_class_bitset.h"


enum class PinFunction_t: std::size_t
{
    // #if BOARD_NAME ESP32S3_DEVKITC_1
    // Header 1
    PIN_4,
    PIN_5,
    PIN_6,
    PIN_7,
    PIN_15,
    PIN_16,
    PIN_17,
    PIN_18,
    PIN_8,
    PIN_3,
    PIN_46,
    PIN_9,
    PIN_10,
    PIN_11,
    PIN_12,
    PIN_13,
    PIN_14,

    // Header 2
    PIN_TX,
    PIN_RX,
    PIN_1,
    PIN_2,
    PIN_42,
    PIN_41,
    PIN_40,
    PIN_39,
    PIN_38,
    PIN_37,
    PIN_36,
    PIN_35,
    PIN_0,
    PIN_45,
    PIN_48,
    PIN_47,
    PIN_21,
    PIN_20,
    PIN_19,

    SIZE
    // #endif
};




class PinManager 
{
    public:
        static PinManager* Instance();

        gpio_num_t reservePin(PinFunction_t pin);
        void unreservePin(PinFunction_t pin);

    protected:
        PinManager();

    private:
        static PinManager *mInstance;
        std::bitset<PinFunction_t::SIZE> pinState;
        // EnumClassBitset<PinFunction_t, PinFunction_t::SIZE> pinState;
        // std::bitset<(size_t)PinFunction_t::SIZE> static pinState;
};



