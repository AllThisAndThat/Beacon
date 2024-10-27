#include "digital_output_object.h"

DigitalOutputObject::DigitalOutputObject()
{
    this->logic = Logic_t::LOW;
    this->isEnabled = false;
    this->isReady = false;
}

esp_err_t DigitalOutputObject::setConfig(DigitalOutputConfig config)
{
    esp_err_t err = ESP_ERR_INVALID_STATE;
    if (this->isEnabled)
    {
        return err;
    }

    err = checkPin(config.pin);
    if (err != ESP_OK) {
        return err;
    }

    this->isReady = false;
    this->config = config;
    return err;
}

esp_err_t DigitalOutputObject::configure()
{
    esp_err_t err = ESP_ERR_INVALID_STATE;
    if (!(this->isEnabled))
    {
        return err;
    }

    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;

    switch (this->config.pullState)
    {
    case (PullState_t::NO_PULL):
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        break;
    case (PullState_t::PULL_UP):
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    case (PullState_t::PULL_DOWN):
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; 
    }

    switch (this->config.mode)
    {
    case (OutputMode_t::PUSH_PULL):
        io_conf.mode = GPIO_MODE_OUTPUT;
        break;
    case (OutputMode_t::OPEN_DRAIN):
        io_conf.mode = GPIO_MODE_OUTPUT_OD;
        break;
    }

    io_conf.pin_bit_mask = (1ULL<<this->config.pin);
    
    err = gpio_config(&io_conf);
    if (err == ESP_OK) {
        this->isReady = true;
    }

    return err;
}

esp_err_t DigitalOutputObject::set_action(Logic_t logic)
{
    esp_err_t err = ESP_ERR_INVALID_STATE;
    if (!(this->isReady))
    {
        return err;
    }

    err = gpio_set_level(this->config.pin, (uint32_t)logic);
    if (err == ESP_OK)
    {
        this->logic = logic;
    }
  
    return err;
}

esp_err_t DigitalOutputObject::set_toggle()
{
    Logic_t tempLogic;
    tempLogic = (this->logic == Logic_t::LOW) ? Logic_t::HIGH : Logic_t::LOW;

    esp_err_t err = ESP_ERR_INVALID_STATE;
    err = set_action(tempLogic);
    if (err == ESP_OK)
    {
        this->logic = tempLogic;
    }

    return err;
}

DigitalOutputConfig DigitalOutputObject::getConfig() const
{
    return this->config; //this should return by value since return type isn't pointer
}

bool DigitalOutputObject::getIfIsEnabled() const
{
    return this->isEnabled;
}

bool DigitalOutputObject::getIfIsReady() const
{
    return this->isReady;
}