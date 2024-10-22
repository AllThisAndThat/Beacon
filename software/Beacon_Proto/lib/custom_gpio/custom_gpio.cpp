#include "custom_gpio.h"

esp_err_t gpio_toggle_level(gpio_num_t pin) 
{
    esp_err_t err;
    if (gpio_get_level(pin) == HIGH)
    {
        err = gpio_set_level(pin, LOW);
    }
    else
    {
        err = gpio_set_level(pin, HIGH);
    }
    return err;
}