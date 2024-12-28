#include "kts1622.h"

#include "reserved_objects.h"


Kts1622::Kts1622(Kts1622Addr addr) 
  : i2c_(reserved_KTS1622::kI2cPort) {
    addr_ = static_cast<uint8_t>(addr);
    last_pullup_mask_port0_   = 0;
    last_pullup_mask_port1_   = 0;
    last_pulldown_mask_port0_ = 0;
    last_pulldown_mask_port1_ = 0;  
}

Kts1622::~Kts1622() {

}

esp_err_t Kts1622::initialize() {
    esp_err_t err = i2c_.initiate();
    if (err != ESP_OK) {return err;}

    err = i2c_.action_add_device(reserved_KTS1622::kDeviceCfg,
                                 hDevice_);
    if (err != ESP_OK) {return err;}

    // SW reset requires its own device
    err = i2c_.action_add_device(reserved_KTS1622::kDeviceRstCfg, hReset_);
    if (err != ESP_OK) {return err;}

    err = action_sw_reset();
    if (err != ESP_OK) {return err;}

    return err;
}

esp_err_t Kts1622::get_input_port0(uint8_t &input_state) {
    constexpr uint8_t rInputStatePort0 = 0x00;
    esp_err_t err = i2c_.action_read(hDevice_, rInputStatePort0,
                                     &input_state);
    if (err != ESP_OK) {return err;}
    return ESP_OK;
}

esp_err_t Kts1622::get_input_port1(uint8_t &input_state) {
    constexpr uint8_t rInputStatePort1 = 0x01;
    esp_err_t err = i2c_.action_read(hDevice_, rInputStatePort1,
                                     &input_state);
    if (err != ESP_OK) {return err;}
    return ESP_OK;
}

esp_err_t Kts1622::set_pullup_state(uint8_t port0_mask,
                                    uint8_t port1_mask) {
    // Set the pullup state                                            
    constexpr uint8_t rPullStatePort0 = 0x48;
    esp_err_t err = i2c_.action_write_reg(hDevice_, rPullStatePort0,
                                      port0_mask);
    if (err != ESP_OK) {return err;}
    constexpr uint8_t rPullStatePort1 = 0x49;
    err = i2c_.action_write_reg(hDevice_, rPullStatePort1, port1_mask);
    if (err != ESP_OK) {return err;}

    // Enable pullups without disabling pulldowns
    uint8_t en_port0 = port0_mask | last_pulldown_mask_port0_;
    uint8_t en_port1 = port1_mask | last_pulldown_mask_port1_;
    last_pullup_mask_port0_ = port0_mask;
    last_pullup_mask_port1_ = port1_mask;
    
    constexpr uint8_t rPullEnPort0 = 0x46;
    err = i2c_.action_write_reg(hDevice_, rPullEnPort0, en_port0);
    if (err != ESP_OK) {return err;}
    constexpr uint8_t rPullEnPort1 = 0x47;
    err = i2c_.action_write_reg(hDevice_, rPullEnPort1, en_port1);
    if (err != ESP_OK) {return err;}

    return err;
}

esp_err_t Kts1622::set_pulldown_state(uint8_t port0_mask,
                                      uint8_t port1_mask) {
    // Set the pulldown state, must invert the mask                                            
    constexpr uint8_t rPullStatePort0 = 0x48;
    esp_err_t err = i2c_.action_write_reg(hDevice_, rPullStatePort0,
                                      ~port0_mask);
    if (err != ESP_OK) {return err;}
    constexpr uint8_t rPullStatePort1 = 0x49;
    err = i2c_.action_write_reg(hDevice_, rPullStatePort1, ~port1_mask);
    if (err != ESP_OK) {return err;}

    // Enable pulldowns without disabling pullups
    uint8_t en_port0 = port0_mask | last_pullup_mask_port0_;
    uint8_t en_port1 = port1_mask | last_pullup_mask_port1_;
    last_pulldown_mask_port0_ = port0_mask;
    last_pulldown_mask_port1_ = port1_mask;

    constexpr uint8_t rPullEnPort0 = 0x46;
    err = i2c_.action_write_reg(hDevice_, rPullEnPort0, en_port0);
    if (err != ESP_OK) {return err;}
    constexpr uint8_t rPullEnPort1 = 0x47;
    err = i2c_.action_write_reg(hDevice_, rPullEnPort1, en_port1);
    if (err != ESP_OK) {return err;}

    return err;
}

esp_err_t Kts1622::action_verify() {
    return i2c_.action_probe(addr_);
}

esp_err_t Kts1622::action_sw_reset() {
    constexpr uint8_t kSwReset = 0x06;
    return i2c_.action_write_direct(hReset_, kSwReset);
}