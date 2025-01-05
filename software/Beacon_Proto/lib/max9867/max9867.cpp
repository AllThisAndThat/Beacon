#include "max9867.h"

#include "reserved_objects.h"


Max9867::Max9867() 
    : i2c_(I2c(reserved_max9867::kI2cPort)) {
}

Max9867::~Max9867() {

}

esp_err_t Max9867::initiate() {
    esp_err_t err = setup_i2c();
    if (err != ESP_OK) {return err;}

    err = setup_mode();
    if (err != ESP_OK) {return err;}

    err = setup_system_clock();
    if (err != ESP_OK) {return err;}

    err = setup_interface_mode();
    if (err != ESP_OK) {return err;}

    err = setup_master_i2s();
    if (err != ESP_OK) {return err;}

    return err;
}

esp_err_t Max9867::get_hp_jack_state(JackState& state) {
    constexpr uint8_t rJackSense = 0x01;
    uint8_t rx_buffer;
    esp_err_t err = i2c_.action_read(hDevice_, rJackSense,
                                     &rx_buffer);
    if (err != ESP_OK) {return err;}

    state = static_cast<JackState>((rx_buffer >> 7));
    return ESP_OK;
}

esp_err_t Max9867::action_probe() {
    return i2c_.action_probe(reserved_max9867::kAddr);
}

esp_err_t Max9867::action_verify() {
    uint8_t rx_buffer = 0x00;
    constexpr uint8_t rRevision = 0xFF;
    esp_err_t err = i2c_.action_read(hDevice_, rRevision,
                                     &rx_buffer);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t kCorrectRevision = 0x42;
    if (rx_buffer != kCorrectRevision) {
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}

esp_err_t Max9867::setup_i2c() {
    esp_err_t err = i2c_.initiate();
    if (err != ESP_OK) {return err;}

    err = i2c_.action_add_device(reserved_max9867::kDeviceCfg,
                                 hDevice_);
    if (err != ESP_OK) {return err;}

    return ESP_OK;
}

esp_err_t Max9867::setup_mode() {
    constexpr uint8_t kDSlewSlow          = (1 << 7);
    constexpr uint8_t kSmoothVolume       = (0 << 6);
    constexpr uint8_t kAtZeroCross        = (0 << 5);
    constexpr uint8_t kJackDetectEn       = (1 << 3);
    constexpr uint8_t kHPMode             = (0b000); // TODO: update to correct value
    constexpr uint8_t kMode = kDSlewSlow | kSmoothVolume
                            | kAtZeroCross | kJackDetectEn
                            | kHPMode;
    constexpr uint8_t rMode = 0x16;
    return i2c_.action_write_reg(hDevice_, rMode, kMode);
}

esp_err_t Max9867::setup_system_clock() {
    constexpr uint8_t kPsclk = 0b01; // TODO: Connect to MCLK reserved somehow
    constexpr uint8_t kFreq  = 0b0000;
    constexpr uint8_t kSystemClk = (kPsclk << 4) | kFreq;
    constexpr uint8_t rSystemClk = 0x05;
    esp_err_t err = i2c_.action_write_reg(hDevice_, rSystemClk,
                                          kSystemClk);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t kLrclkProvided = (1 << 7);
    constexpr uint8_t rAudioClkHigh = 0x06;
    err = i2c_.action_write_reg(hDevice_, rAudioClkHigh,
                                kLrclkProvided);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t kNoNi = 0;
    constexpr uint8_t rAudioClkLow = 0x07;
    err = i2c_.action_write_reg(hDevice_, rAudioClkLow, kNoNi);
    if (err != ESP_OK) {return err;}

    return ESP_OK;
}

esp_err_t Max9867::setup_interface_mode() {
    // Page 26
    constexpr uint8_t kMas_slaveMode   = 0;
    constexpr uint8_t kWci_noInvert    = 0;
    constexpr uint8_t kBci_risingEdge  = 0;
    constexpr uint8_t kDly_philips     = 1;
    constexpr uint8_t kHizoff_noHiz    = 1;
    constexpr uint8_t kTdm_off         = 0;
    constexpr uint8_t kInterfaceReg1 = (kMas_slaveMode  << 7) | 
                                       (kWci_noInvert   << 6) |
                                       (kBci_risingEdge << 5) |
                                       (kDly_philips    << 4) |
                                       (kHizoff_noHiz   << 3) |
                                       (kTdm_off        << 2);

    constexpr uint8_t rInterfaceReg1 = 0x08;
    esp_err_t err = i2c_.action_write_reg(hDevice_, rInterfaceReg1,
                                          kInterfaceReg1);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t kLvoFix_track  = 0;
    constexpr uint8_t kDmono_mono    = 1;
    constexpr uint8_t kBsel_notUsed  = 0b000;
    constexpr uint8_t kInterfaceReg2 = (kLvoFix_track  << 4) | 
                                       (kDmono_mono    << 3) |
                                       (kBsel_notUsed  << 0);
    constexpr uint8_t rInterfaceReg2 = 0x09;                                   
    err = i2c_.action_write_reg(hDevice_, rInterfaceReg2,
                                         kInterfaceReg2);
    if (err != ESP_OK) {return err;}

    return ESP_OK;
    // constexpr uint8_t kNsdodly_inPhase = 0; // TODO: need to write interrupt register
}

esp_err_t Max9867::setup_master_i2s() {
    esp_err_t err = i2s_new_channel(&reserved_i2s0::kChanCfg,
                                    &hI2sTx_, &hI2sRx_);
    if (err != ESP_OK) {return err;}

    err = i2s_channel_init_std_mode(hI2sTx_, &reserved_i2s0::kStdCfg);
    if (err != ESP_OK) {return err;}
    err = i2s_channel_init_std_mode(hI2sRx_, &reserved_i2s0::kStdCfg);
    if (err != ESP_OK) {return err;}

    err = i2s_channel_enable(hI2sTx_);
    if (err != ESP_OK) {return err;}
    err = i2s_channel_enable(hI2sRx_);
    if (err != ESP_OK) {return err;}

    return err;
}