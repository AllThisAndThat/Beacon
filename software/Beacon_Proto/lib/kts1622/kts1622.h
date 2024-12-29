#pragma once

#include "esp_err.h"

#include "i2c.h"
#include "driver/gpio.h"

/*
Want to suport the following
- whats best? mask or individual pins
- interrupt setup - setup interrupt on kts1622 side. Other device grabbing data needs to setup interrupt on ESP side
- switch debouncing - have not tested this yet
- open drain support
*/

enum class  Kts1622Addr: uint8_t {
    kDefault = 0x20,
    kVss     = 0x20,
    kVdd     = 0x21,
    VScl     = 0x22,
    VSda     = 0x23
};

namespace Kts1622Pin {
    enum Kts1622Pin: uint8_t {
        kNoPin  = 0x00,
        kPin0   = 0x01,
        kPin1   = 0x02,
        kPin2   = 0x04,
        kPin3   = 0x08,
        kPin4   = 0x10,
        kPin5   = 0x20,
        kPin6   = 0x40,
        kPin7   = 0x80,
        kAllPin = 0xFF
    };
}

class Kts1622 {
public:
    Kts1622(Kts1622Addr addr);
    ~Kts1622();
    esp_err_t initialize();

    esp_err_t get_input_port0(uint8_t &input_state);
    esp_err_t get_input_port1(uint8_t &input_state);

    esp_err_t set_pullup_state(uint8_t port0_mask, 
                               uint8_t port1_mask);
    esp_err_t set_pulldown_state(uint8_t port0_mask,
                                 uint8_t port1_mask);
    esp_err_t set_pin_direction(uint8_t port0_mask, 
                                uint8_t port1_mask);
    esp_err_t set_output_state(uint8_t port0_mask, 
                               uint8_t port1_mask);
    esp_err_t set_interrupt_state(uint8_t port0_mask, 
                                  uint8_t port1_mask);
    esp_err_t set_debounce_state(uint8_t port0_mask, 
                                 uint8_t port1_mask, 
                                 uint8_t num_cycles);

    esp_err_t action_verify();
    esp_err_t action_sw_reset();

private:
    I2c i2c_;
    i2c_master_dev_handle_t hDevice_;
    i2c_master_dev_handle_t hReset_;

    uint8_t addr_;

    uint8_t last_pullup_mask_port0_;
    uint8_t last_pullup_mask_port1_;
    uint8_t last_pulldown_mask_port0_;
    uint8_t last_pulldown_mask_port1_;
};