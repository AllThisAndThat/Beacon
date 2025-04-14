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

    err = action_verify();
    if (err != ESP_OK) {return err;}

    err = setup_main_control();
    if (err != ESP_OK) {return err;}

    err = setup_sensitivity_control();
    if (err != ESP_OK) {return err;}

    err = setup_sensor_input_enable();
    if (err != ESP_OK) {return err;}

    err = setup_sensor_input_config();
    if (err != ESP_OK) {return err;}

    err = setup_avg_sample_config();
    if (err != ESP_OK) {return err;}

    err = setup_recal_config();
    if (err != ESP_OK) {return err;}

    constexpr configSTACK_DEPTH_TYPE kStackSize = 10'000;
    using namespace reserved;
    xTaskCreatePinnedToCore(static_cast<TaskFunction_t>(this->vTask), 
                            "CAP1188", kStackSize, this, 
                            priority::kCAP1188,
                            &hTask_, core::kCAP1188);

    return err;
}

esp_err_t CAP1188::get_input() {
    esp_err_t err = ESP_OK;

    uint8_t kReadData;

    constexpr uint8_t rDelta1 = 0x10;
    err = i2c_.action_read(hDevice_, rDelta1, &kReadData);
    if (err != ESP_OK) {return err;}
    printf(">Data1: %d\n", kReadData);
    int8_t delta = static_cast<int8_t>(kReadData);

    constexpr uint8_t rCal1_1 = 0xB1;
    err = i2c_.action_read(hDevice_, rCal1_1, &kReadData);
    if (err != ESP_OK) {return err;}
    printf(">Data2: %d\n", kReadData);
    uint8_t kCal1_1 = kReadData;

    constexpr uint8_t rCal1_2 = 0xB9;
    err = i2c_.action_read(hDevice_, rCal1_2, &kReadData);
    if (err != ESP_OK) {return err;}
    printf(">Data3: %d\n", kReadData);
    uint8_t kCal1_2 = kReadData;

    int16_t temp = ((kCal1_1 << 2) | (kCal1_2 && 0b11)) + delta;
    printf(">Data4: %d\n", temp);;

    return err;
}

esp_err_t CAP1188::action_verify() {
    constexpr uint8_t addr = reserved::i2c0::kAddr_CAP1188;
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

esp_err_t CAP1188::setup_main_control() {
    // Page 32
    constexpr uint8_t kGAIN_1 = 0b00 << 6;
    constexpr uint8_t kSTBY_disbaled = 0b0 << 5;
    constexpr uint8_t kSLEEP_disabled = 0b0 << 4;
    constexpr uint8_t kINT_disabled = 0b0;
    constexpr uint8_t kMainControl = kGAIN_1 | kSTBY_disbaled | kSLEEP_disabled | kINT_disabled;
    constexpr uint8_t rMainControl = 0x00;
    return i2c_.action_write_reg(hDevice_, rMainControl, kMainControl);
}

esp_err_t CAP1188::setup_sensitivity_control() {
    // Page 35
    constexpr uint8_t kDELTA_SENSE_8 = 0b100 << 4;
    constexpr uint8_t kBASE_SHIFT_1 = 0b1111;
    constexpr uint8_t kSensitivity = kDELTA_SENSE_8 | kBASE_SHIFT_1;
    constexpr uint8_t rSensitivity = 0x1F;
    return i2c_.action_write_reg(hDevice_, rSensitivity, kSensitivity);
}

esp_err_t CAP1188::setup_sensor_input_enable() {
    // Page 38
    constexpr uint8_t kCS8_dis = 0 << 7;
    constexpr uint8_t kCS7_dis = 0 << 6;
    constexpr uint8_t kCS6_en  = 1 << 5;
    constexpr uint8_t kCS5_en  = 1 << 4;
    constexpr uint8_t kCS4_en  = 1 << 3;
    constexpr uint8_t kCS3_en  = 1 << 2;
    constexpr uint8_t kCS2_en  = 1 << 1;
    constexpr uint8_t kCS1_en  = 1 << 0;
    constexpr uint8_t kSensorInputEnable = kCS8_dis  | kCS7_dis 
                                           | kCS6_en | kCS5_en 
                                           | kCS4_en | kCS3_en
                                           | kCS2_en | kCS1_en;
    constexpr uint8_t rSensorInputEnable = 0x21;
    return i2c_.action_write_reg(hDevice_, rSensorInputEnable, kSensorInputEnable);
}

esp_err_t CAP1188::setup_sensor_input_config() {
    // Page 39
    constexpr uint8_t kMAX_DUR_560ms = 0b0000 << 4;
    constexpr uint8_t kRPT_RATE_default = 0b0100;
    constexpr uint8_t kSensorInputConfig = kMAX_DUR_560ms | kRPT_RATE_default;
    constexpr uint8_t rSensorInputConfig = 0x22;
    return i2c_.action_write_reg(hDevice_, rSensorInputConfig, kSensorInputConfig);
}

esp_err_t CAP1188::setup_avg_sample_config() {
    // Page 40
    constexpr uint8_t kAVG_1 = 0b000 << 4;
    constexpr uint8_t kSAMP_TIME_320us = 0b00 << 2;
    constexpr uint8_t kCYCLE_TIME_35ms = 0b00;
    constexpr uint8_t kSampleConfig = kAVG_1 | kSAMP_TIME_320us
                                      | kCYCLE_TIME_35ms;
    constexpr uint8_t rSampleConfig = 0x24;
    return i2c_.action_write_reg(hDevice_, rSampleConfig, kSampleConfig);
}

esp_err_t CAP1188::setup_recal_config() {
    // Page 45
    constexpr uint8_t kBUT_LD_TH_default = 0b1 << 7;
    constexpr uint8_t kNO_CLR_INTD_dis = 0b0 << 6;
    constexpr uint8_t kNO_CLR_NEG_dis = 0b0 << 5;
    constexpr uint8_t kNEG_DELTA_CNT_dis = 0b11 << 3;
    constexpr uint8_t kCAL_CFG_default = 0b010;
    constexpr uint8_t kRecalConfig = kBUT_LD_TH_default | kNO_CLR_INTD_dis
                                    | kNO_CLR_NEG_dis | kNEG_DELTA_CNT_dis
                                    | kCAL_CFG_default;
    constexpr uint8_t rRecalConfig = 0x26;
    return i2c_.action_write_reg(hDevice_, rRecalConfig, kRecalConfig);
}

void CAP1188::vTask(void *pvParameters) {
    CAP1188* cap1188 = static_cast<CAP1188*>(pvParameters);
    for (;;) {
        cap1188->get_input();
        vTaskDelay(1/portTICK_PERIOD_MS);
    }
}