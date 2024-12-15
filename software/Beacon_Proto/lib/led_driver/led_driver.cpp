#include "led_driver.h"

constexpr uint32_t kLedFrequency = 4000; // Frequency in Hertz.
constexpr ledc_timer_bit_t kResolution = LEDC_TIMER_10_BIT;
constexpr uint32_t kOff = 0;

LedDriver::LedDriver() {
    this->pin = GPIO_NUM_NC;
    this->timer = LEDC_TIMER_MAX;
    this->channel = LEDC_CHANNEL_MAX;
}

LedDriver::~LedDriver() {

}

esp_err_t LedDriver::initiate(gpio_num_t gpio_num, ledc_timer_t timer,
                              ledc_channel_t channel, Logic logic) {

    esp_err_t err = this->configure_init_timer(timer);
    if (err != ESP_OK) {
        return err;
    }

    err = this->configure_init_channel(gpio_num, channel, logic);
    if (err != ESP_OK) {
        return err;
    }

    ledc_fade_func_install(0); // Don't understand this

    this->pin     = gpio_num;
    this->timer   = timer;
    this->channel = channel;
    return err;
}

esp_err_t LedDriver::set_duty(uint32_t duty) {
    constexpr uint32_t kMaxDuty = 1000;
    if (duty > kMaxDuty) {
        this->duty = kOff;
        return ESP_ERR_INVALID_ARG;
    }
    constexpr uint32_t kResolutionFactor = static_cast<uint32_t>(kResolution);
    this->duty = (duty << kResolutionFactor) / kMaxDuty; // Dynamically multiply by the resolution. Shapes (0 - 1000) -> (0 - Max resolution)
    return ESP_OK;
}

esp_err_t LedDriver::action_off() {
    constexpr int kNoHPoint = 0;
    return ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, 
        this->channel, kOff, kNoHPoint);
}

esp_err_t LedDriver::action_on() {
    constexpr int kNoHPoint = 0;
    return ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, 
        this->channel, duty, kNoHPoint);
}

esp_err_t LedDriver::configure_init_timer(ledc_timer_t timer) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = kResolution,
        .timer_num = timer, 
        .freq_hz = kLedFrequency,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false
    };
    esp_err_t err = ledc_timer_config(&ledc_timer);
    if (err == ESP_OK)
    {
        this->timer = timer; 
    }
    return err;
}

esp_err_t LedDriver::configure_init_channel(gpio_num_t gpio_num,
                                            ledc_channel_t channel,
                                            Logic logic) {

    constexpr uint32_t kInitialDuty = 0;
    constexpr int kInitialHPoint = 0;
    ledc_channel_config_t ledc_channel = {
        .gpio_num = static_cast<int>(gpio_num),
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = this->timer,
        .duty = kInitialDuty,
        .hpoint = kInitialHPoint,
        .flags {
            .output_invert = static_cast<unsigned int>(logic)
        }
    };
    esp_err_t err = ledc_channel_config(&ledc_channel);
    if (err == ESP_OK)
    {
        this->channel = channel; 
    }
    return err;
}