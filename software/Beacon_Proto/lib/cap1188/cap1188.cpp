#include "cap1188.h"

CAP1188::CAP1188()
  : i2c_(reserved::CAP1188::kI2cPort) {
    
}

CAP1188::~CAP1188() {
    
}

esp_err_t CAP1188::initiate() {
    
    esp_err_t err = i2c_.initiate();
    if (err != ESP_OK) {return err;}
    
    err = i2c_.action_add_device(reserved::CAP1188::kDeviceCfg,
                                 hDevice_);
    if (err != ESP_OK) {return err;}
    
    return err;
}

esp_err_t CAP1188::get_input() {
    esp_err_t err = ESP_OK;
    constexpr uint8_t rData1 = 0x10;
    uint8_t kReadData;
    err = i2c_.action_read(hDevice_, rData1, &kReadData);
    if (err != ESP_OK) {return err;}
    int8_t kSignedData = static_cast<int8_t>(kReadData);
    printf(">DeltaCount: %d\n", kSignedData);

    return err;
}

esp_err_t CAP1188::action_verify() {
    constexpr uint8_t addr = reserved::i2c1::kAddr_CAP1188;
    esp_err_t err = i2c_.action_probe(addr);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t rProductId = 0xFD;
    uint8_t kReadId;
    err = i2c_.action_read(hDevice_, rProductId, &kReadId);
    if (err != ESP_OK) {return err;}
    constexpr uint8_t kProductId = 0x50;
    if (kReadId != kProductId) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    constexpr uint8_t rManufacturId = 0xFE;
    err = i2c_.action_read(hDevice_, rManufacturId, &kReadId);
    if (err != ESP_OK) {return err;}
    constexpr uint8_t kManufactureId = 0x5D;
    if (kReadId != kManufactureId) {
        return ESP_ERR_INVALID_RESPONSE;
    } 

    constexpr uint8_t rICRevision = 0xFF;
    err = i2c_.action_read(hDevice_, rICRevision, &kReadId);
    if (err != ESP_OK) {return err;}
    constexpr uint8_t kICRevision = 0x83;
    if (kReadId != kICRevision) {
        return ESP_ERR_INVALID_RESPONSE;
    } 

    return err;
}