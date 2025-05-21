/*
https://lumissil.com/assets/pdf/core/IS31FL3741A_DS.pdf
https://www.adafruit.com/product/5201
*/

#pragma once

#include "stm32h5xx_hal.h"

#include "i2c_master.h"

enum class Page: uint8_t {
    // Page 9 datasheet
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
    uint32_t x;
    uint32_t y;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class IS31FL3741 {
public:
    IS31FL3741();
    ~IS31FL3741();
    HAL_StatusTypeDef initiate();

    HAL_StatusTypeDef set_global_current(const uint8_t current);
    HAL_StatusTypeDef set_specific_led(uint32_t led_num, const uint8_t pwm,
                               const uint8_t scale);
    HAL_StatusTypeDef set_rgb_led(LedConfig config);

    HAL_StatusTypeDef action_verify();
    HAL_StatusTypeDef action_off();
    HAL_StatusTypeDef action_on();
    HAL_StatusTypeDef action_reset_all_leds();

private:
    I2c i2c_;
    Page page_;
    IS31FL3741_State state_;

    uint8_t global_current_;

    HAL_StatusTypeDef setup_config_register();
    HAL_StatusTypeDef setup_pull_resistor();

    HAL_StatusTypeDef set_page(const Page page);

    HAL_StatusTypeDef action_write_page(Page page, uint8_t* data,
                                const size_t data_len);
};