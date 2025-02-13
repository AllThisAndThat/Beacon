#pragma once

#include "esp_err.h"

#include "i2c.h"

enum class Page: uint8_t {
    // Page 9
    k0 = 0b0000'0000,
    k1 = 0b0000'0001,
    k2 = 0b0000'0010,
    k3 = 0b0000'0011,
    k4 = 0b0000'0100
};

enum class IS31FL3741_State : int {
    kOff = 0,
    kOn = 1
};

// enum class Color: uint8_t {
//     kRed   = 2,
//     kGreen = 1,
//     kBlue  = 0,
// };


struct LedConfig {
    uint8_t x;
    uint8_t y;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class IS31FL3741 {
public:
    IS31FL3741();
    ~IS31FL3741();
    esp_err_t initiate();

    esp_err_t set_global_current(const uint8_t current);
    esp_err_t set_specific_led(uint32_t led_num, const uint8_t pwm,
                               const uint8_t scale);
    esp_err_t set_rgb_led(LedConfig config);

    esp_err_t action_verify();
    esp_err_t action_off();
    esp_err_t action_on();
    esp_err_t action_reset_all_leds();

private:
    I2c i2c_;
    i2c_master_dev_handle_t hDevice_;
    Page page_;
    IS31FL3741_State state_;

    uint8_t global_current_;

    esp_err_t setup_config_register();
    esp_err_t setup_pull_resistor();

    esp_err_t set_page(const Page page);

    esp_err_t action_write_page(Page page, uint8_t* data,
                                const size_t data_len);
};
