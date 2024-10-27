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

