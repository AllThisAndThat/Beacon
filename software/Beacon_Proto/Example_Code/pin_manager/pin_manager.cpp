#include "pin_manager.h"

PinManager* PinManager::mInstance = 0;

uint32_t PinManager::myBitSet = 0;

PinManager* PinManager::Instance()
{
    if (mInstance == 0)
    {
        mInstance = new PinManager;
    }
    return mInstance;
}

esp_err_t PinManager::enableDigitalOutputObject(DigitalOutputObject* object)
{
    uint32_t oneHot = (uint32_t)1 << (object->config.pin);

    // Test if pin is already in use
    if ((this->myBitSet) & (oneHot) != 0)
    {
        return ESP_ERR_INVALID_STATE;
    }

    // If not in use, grant permission to object
    this->myBitSet = this->myBitSet | (oneHot);
    object->isEnabled = true;

    return ESP_OK;
}

esp_err_t PinManager::disableDigitalOutputObject(DigitalOutputObject* object)
{
    if (!(object->isEnabled))
    {
        return ESP_ERR_INVALID_STATE;
    }

    uint32_t oneHot = (uint32_t)1 << (object->config.pin);

    (this->myBitSet) = (this->myBitSet) - oneHot;
    object->isEnabled = false;

    return ESP_OK;
}