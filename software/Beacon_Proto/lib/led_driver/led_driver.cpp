#include "led_driver.h"

constexpr uint32_t kLedFrequency = 4000; // Frequency in Hertz.

uint32_t LedDriver::reserved_timer_mask = 0b0000;

LedDriver::LedDriver() {
    this->isInitiated = false;
}

esp_err_t LedDriver::initiate(gpio_num_t gpio_num) {
    esp_err_t err = this->configure_init_timer();
    if (err != ESP_OK) {
        return err;
    }

    err = this->configure_init_channel(gpio_num);
    if (err != ESP_OK) {
        return err;
    }

    this->isInitiated = true;
    this->pin = gpio_num;
    return err;
}

esp_err_t LedDriver::set_duty(uint32_t duty) {
    esp_err_t err = ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    if (err != ESP_OK)
    {
        return err;
    }
    err = ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    if (err == ESP_OK)
    {
        this->duty = duty;
    }
    return err;
}

esp_err_t LedDriver::configure_init_timer() {
    constexpr int kAllTimersReservedMask = 0b1111;
    if (reserved_timer_mask == kAllTimersReservedMask)
    {
        return ESP_ERR_INVALID_STATE;
    }

    ledc_timer_t chosen_timer = this->find_timer();
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = chosen_timer, 
        .freq_hz = kLedFrequency,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false
    };
    esp_err_t err = ledc_timer_config(&ledc_timer);
    if (err == ESP_OK)
    {
        this->timer = chosen_timer; 
    }
    return err;
}

esp_err_t LedDriver::configure_init_channel(gpio_num_t gpio_num) const {
    constexpr uint32_t kInitialDuty = 0;
    constexpr int kInitialHPoint = 0;
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = static_cast<int>(gpio_num),
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0, // always channel 0 for each timer (for now)
        .intr_type      = LEDC_INTR_DISABLE, 
        .timer_sel      = this->timer, 
        .duty           = kInitialDuty, 
        .hpoint         = kInitialHPoint 
    };
    return ledc_channel_config(&ledc_channel);
}

ledc_timer_t LedDriver::find_timer() {
    /*
    Returns the lowest timer not used. Reserves timer.
    */
    int i, mask;
    constexpr int MAX = static_cast<int>(LEDC_TIMER_MAX) - 1;
    for (i = 0; i < MAX; i++)
    {
        mask = (1 << i);
        if ((reserved_timer_mask & mask) == 0) {
            reserved_timer_mask |= mask;
            break;
        }
    }
    return static_cast<ledc_timer_t>(i);
}
