#pragma once

#include "esp_err.h"

#include "i2c.h"

/*
Want to suport the following
- interrupt setup
- switch debouncing
*/

enum class  Kts1622Addr: uint8_t {
    kDefault = 0x20,
    kVss     = 0x20,
    kVdd     = 0x21,
    VScl     = 0x22,
    VSda     = 0x23
};

class Kts1622 {
public:
    Kts1622(Kts1622Addr addr);
    ~Kts1622();
    esp_err_t initialize();

    esp_err_t get_input_port0(uint8_t &input_state);
    esp_err_t get_input_port1(uint8_t &input_state);

    esp_err_t set_pullup_state(uint8_t port0_mask, uint8_t port1_mask);
    esp_err_t set_pulldown_state(uint8_t port0_mask, uint8_t port1_mask);

    esp_err_t action_verify();
    esp_err_t action_sw_reset();

    // Fixme: after testing, add to private

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