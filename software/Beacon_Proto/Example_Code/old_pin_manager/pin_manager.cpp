#include "pin_manager.h"

PinManager* PinManager::mInstance = 0;

PinManager *PinManager::Instance()
{
    if (mInstance == 0)
    {
        mInstance = new PinManager;
    }
    return mInstance;
}

gpio_num_t PinManager::reservePin(PinFunction_t pin)
{
    gpio_num_t ret = GPIO_NUM_NC;

    if (pinState.test(pin)) 
    {
        return GPIO_NUM_NC;
    }

    using enum PinFunction_t;
    switch (pin)
    {
        case (PIN_4):   ret = GPIO_NUM_4;
        case (PIN_5):   ret = GPIO_NUM_5;
        case (PIN_6):   ret = GPIO_NUM_6;
        case (PIN_7):   ret = GPIO_NUM_7;
        case (PIN_15):  ret = GPIO_NUM_15;
        case (PIN_16):  ret = GPIO_NUM_16;
        case (PIN_17):  ret = GPIO_NUM_17;
        case (PIN_18):  ret = GPIO_NUM_18;
        case (PIN_8):   ret = GPIO_NUM_8;
        case (PIN_3):   ret = GPIO_NUM_3;
        case (PIN_46):  ret = GPIO_NUM_46;
        case (PIN_9):   ret = GPIO_NUM_9;
        case (PIN_10):  ret = GPIO_NUM_10; 
        case (PIN_11):  ret = GPIO_NUM_11;
        case (PIN_12):  ret = GPIO_NUM_12;
        case (PIN_13):  ret = GPIO_NUM_13;            
        case (PIN_14):  ret = GPIO_NUM_14;

        case (PIN_TX):  ret = GPIO_NUM_43;
        case (PIN_RX):  ret = GPIO_NUM_44;
        case (PIN_1):   ret = GPIO_NUM_1;
        case (PIN_2):   ret = GPIO_NUM_2;
        case (PIN_42):  ret = GPIO_NUM_42;
        case (PIN_41):  ret = GPIO_NUM_41;
        case (PIN_40):  ret = GPIO_NUM_40;
        case (PIN_39):  ret = GPIO_NUM_39;
        case (PIN_38):  ret = GPIO_NUM_38;
        case (PIN_37):  ret = GPIO_NUM_37;
        case (PIN_36):  ret = GPIO_NUM_36;
        case (PIN_35):  ret = GPIO_NUM_35;
        case (PIN_0):   ret = GPIO_NUM_0;
        case (PIN_45):  ret = GPIO_NUM_45;
        case (PIN_48):   ret = GPIO_NUM_48;
        case (PIN_47):  ret = GPIO_NUM_47;
        case (PIN_21):  ret = GPIO_NUM_21;
        case (PIN_20):  ret = GPIO_NUM_20;
        case (PIN_19):  ret = GPIO_NUM_19;

        default:
            abort();
    }
    
    pinState.set(pin);
    return ret;
}

void PinManager::unreservePin(PinFunction_t pin) 
{
    pinState.reset(pin);
}
