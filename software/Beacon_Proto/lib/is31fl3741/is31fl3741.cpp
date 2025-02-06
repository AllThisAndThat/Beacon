#include "is31fl3741.h"

namespace IS31FL3741_Config {
    constexpr uint8_t kSWS_n9        = (0b0000 << 4);
    constexpr uint8_t kLGC_2V4_0V6   = (1 << 3);
    constexpr uint8_t kOSDE_noDetect = (0b00 << 1);
    
    constexpr uint8_t kData = kSWS_n9 | kLGC_2V4_0V6 | kOSDE_noDetect;
    constexpr uint8_t kDataOff = kData | (0 << 0);
    constexpr uint8_t kDataOn  = kData | (1 << 0);
    constexpr uint8_t rRegister = 0x00;
}

IS31FL3741::IS31FL3741()
  : i2c_(reserved::IS31FL3741::kI2cPort) {
    using namespace IS31FL3741_Config;  
}

IS31FL3741::~IS31FL3741() {

}

esp_err_t IS31FL3741::initiate() {
    using namespace reserved::IS31FL3741;

    esp_err_t err = i2c_.initiate();
    if (err != ESP_OK) {return err;}

    err = i2c_.action_add_device(reserved::IS31FL3741::kDeviceCfg,
                                 hDevice_);
    if (err != ESP_OK) {return err;}

    esp_err_t err = action_unlock(Page::k4);
    if (err != ESP_OK) {return err;}

    err = setup_config_register();
    if (err != ESP_OK) {return err;}

    err = setup_pull_resistor();
    if (err != ESP_OK) {return err;}

    return err;
}

esp_err_t IS31FL3741::action_verify() {
    constexpr uint8_t addr = reserved::i2c1::kAddr_IS31FL3741;
    esp_err_t err = i2c_.action_probe(addr);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t kCorrectId = 0x60;
    constexpr uint8_t rId = 0xFC;
    uint8_t kReadId;
    err = i2c_.action_read(hDevice_, rId, &kReadId);
    if (err != ESP_OK) {return err;}

    if (kReadId != kCorrectId) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    return err;
}

esp_err_t IS31FL3741::action_off() {
    using namespace IS31FL3741_Config;
    return i2c_.action_write_reg(hDevice_, rRegister, kDataOff);
}

esp_err_t IS31FL3741::action_on() {
    using namespace IS31FL3741_Config;
    return i2c_.action_write_reg(hDevice_, rRegister, kDataOn);
}

esp_err_t IS31FL3741::setup_config_register() {
    constexpr uint8_t kSWS_n9        = (0b0000 << 4);
    constexpr uint8_t kLGC_2V4_0V6   = (1 << 3);
    constexpr uint8_t kOSDE_noDetect = (0b00 << 1);
    constexpr uint8_t kSSD_normal    = (1 << 0);
    constexpr uint8_t kConfig = kSWS_n9 | kLGC_2V4_0V6 |
                                kOSDE_noDetect | kSSD_normal;
    constexpr uint8_t rConfig = 0x00;
    return i2c_.action_write_reg(hDevice_, rConfig, kConfig);
}

esp_err_t IS31FL3741::setup_pull_resistor() {
    constexpr uint8_t kPDR_32k = (0b111 << 4);
    constexpr uint8_t kPUR_32k = (0b111 << 0);
    constexpr uint8_t kPullResistor = kPDR_32k | kPUR_32k;
    constexpr uint8_t rPullResistor = 0x02;
    return i2c_.action_write_reg(hDevice_, rPullResistor, kPullResistor);
}

esp_err_t IS31FL3741::action_unlock(Page page) {
    constexpr uint8_t kUnlock = 0b1100'0101;
    constexpr uint8_t rUnlock = 0xFE;
    esp_err_t err = i2c_.action_write_reg(hDevice_, rUnlock, kUnlock);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t rPageSelect = 0xFD;
    return i2c_.action_write_reg(hDevice_, rPageSelect, 
                                 static_cast<uint8_t>(page));
}
