#include "bmi323.h"

#include <string.h>

#include "reserved_objects.h"

namespace BMI323Cmd {
    enum BMI323Cmd: uint16_t {
        kWrite = 0x00,
        kRead  = 0x01
    };
};

BMI323::BMI323() 
    : spi_(Spi(reserved::BMI323::port)) {

}

BMI323::~BMI323() {

}

esp_err_t BMI323::initiate() {
    // Page 15
    esp_err_t err = spi_.initiate();
    if (err != ESP_OK) {return err;}

    err = action_verify();
    if (err != ESP_OK) {printf("Verify Failed\n"); return err;}
    
    err = setup_accelerometer();
    if (err != ESP_OK) {printf("Acc Setup Failed\n"); return err;}

    err = setup_gyro();
    if (err != ESP_OK) {printf("Gyro Setup Failed\n"); return err;}

    constexpr configSTACK_DEPTH_TYPE kStackSize = 10'000;
    using namespace reserved;
    xTaskCreatePinnedToCore(static_cast<TaskFunction_t>(this->vTask), 
                "IMU", kStackSize, this, priority::kBMI323,
                &hTask_, core::kBMI323);
    return err;
}

esp_err_t BMI323::action_verify() {
    // Check chip id
    // Page 64
    constexpr uint8_t rChipId = 0x00;
    uint8_t data[2] = {0, 0};
    esp_err_t err = action_read_single(rChipId, data);
    if (err != ESP_OK) { return err;}
    constexpr uint8_t kCorrectChipId = 0x43;
    if (data[0] != kCorrectChipId) {
        printf("Chip ID (expect 0x43): %d, %d\n", data[0], data[1]);
        return ESP_ERR_INVALID_RESPONSE;
    }

    // Check chip is in operational state, no errors
    // Page 65
    constexpr uint8_t rErrorStatus = 0x01;
    err = action_read_single(rErrorStatus, data);
    if (err != ESP_OK) { return err;}
    constexpr uint8_t kOperationalState = 0;
    if (data[0] != kOperationalState) {
        printf("Error Status (expect 0): %d, %d\n", data[0], data[1]);
        return ESP_ERR_INVALID_STATE;
    }

    // Seems optional. Check if was just reset
    // Page 66
    // constexpr uint8_t rDeviceStatus = 0x02;
    // err = action_read(rDeviceStatus, data);
    // if (err != ESP_OK) { return err;}
    // constexpr uint8_t kDeviceReady = 0b1;
    // if (data[0] != kDeviceReady) {
    //     printf("Device (expect 0b1): %d, %d\n", data[0], data[1]);
    //     // return ESP_ERR_INVALID_STATE;
    // }

    return err;
}

esp_err_t BMI323::get_accelerometer_data(int16_t acc_data[3]) const {
    // Page 67
    constexpr int kLength = 6;
    uint8_t data[kLength];

    constexpr uint8_t rAccX = 0x03;
    esp_err_t err = action_read_multi(rAccX, data, kLength);
    if (err != ESP_OK) { return err;}
    acc_data[0] = static_cast<int16_t>((data[1] << 8) | data[0]);
    acc_data[1] = static_cast<int16_t>((data[3] << 8) | data[2]);
    acc_data[2] = static_cast<int16_t>((data[5] << 8) | data[4]);

    printf(">Acc_X: %d\n", acc_data[0]);
    printf(">Acc_Y: %d\n", acc_data[1]);
    printf(">Acc_Z: %d\n", acc_data[2]);

    return err;
}

esp_err_t BMI323::get_gyro_data(int16_t gyr_data[3]) const {
    // Page 70
    constexpr int kLength = 6;
    uint8_t data[kLength];

    constexpr uint8_t rGyrX = 0x06;
    esp_err_t err = action_read_multi(rGyrX, data, kLength);
    if (err != ESP_OK) { return err;}
    gyr_data[0] = static_cast<int16_t>((data[1] << 8) | data[0]);
    gyr_data[1] = static_cast<int16_t>((data[3] << 8) | data[2]);
    gyr_data[2] = static_cast<int16_t>((data[5] << 8) | data[4]);

    printf(">Gyr_X: %d\n", gyr_data[0]);
    printf(">Gyr_Y: %d\n", gyr_data[1]);
    printf(">Gyr_Z: %d\n", gyr_data[2]);

    return err;
}

esp_err_t BMI323::setup_accelerometer() const {
    // Page 89
    constexpr uint8_t kAccBw_div2   = (0b0    << 7);
    constexpr uint8_t kAccRange_4g  = (0b001  << 4);
    constexpr uint8_t kAccOdr_50Hz  = (0b0111 << 0);
    constexpr uint8_t kAccConf0 = kAccBw_div2 | kAccRange_4g | kAccOdr_50Hz;

    constexpr uint8_t kAccMode_norm   = (0b100 << 4);
    constexpr uint8_t kAccAvgNum_4Avg = (0b010 << 0);
    constexpr uint8_t kAccConf1 = kAccMode_norm | kAccAvgNum_4Avg;

    constexpr uint64_t rAccConf = 0x20;
    uint8_t data[2] = {kAccConf0, kAccConf1};
    return action_write(rAccConf, data);
}

esp_err_t BMI323::setup_gyro() const {
    // Page 91
    constexpr uint8_t kGyrBw_div2       = (0b0    << 7);
    constexpr uint8_t kGyrRange_500dps  = (0b010  << 4);
    constexpr uint8_t kGyrOdr_50Hz      = (0b0111 << 0);
    constexpr uint8_t kGyrConf0 = kGyrBw_div2 | kGyrRange_500dps |
                                  kGyrOdr_50Hz;

    constexpr uint8_t kGyrMode_norm   = (0b100 << 4);
    constexpr uint8_t kGyrAvgNum_4Avg = (0b010 << 0);
    constexpr uint8_t kGyrConf1 = kGyrMode_norm | kGyrAvgNum_4Avg;

    constexpr uint64_t rGyrConf = 0x21;
    uint8_t data[2] = {kGyrConf0, kGyrConf1};
    return action_write(rGyrConf, data);
}

esp_err_t BMI323::action_write(const uint8_t addr, const uint8_t data[2]) const{
    spi_transaction_t t {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 24,
        .tx_data = {addr, data[0], data[1] , 0},
        .rx_buffer = NULL
    };
    return spi_.action_polling_tx(t);
}

esp_err_t BMI323::action_read_single(const uint8_t addr, uint8_t data[2]) const {
    uint8_t CMD_and_addr = static_cast<uint8_t>(0b1000'0000) | addr;
    constexpr uint8_t kDummy = 0b0000'0000;
    spi_transaction_t t {
        .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA,
        .length = 32,
        .rxlength = 2 * 8,
        .tx_data = {CMD_and_addr, kDummy},
    };
    esp_err_t err = spi_.action_polling_tx(t); 
    data[0] = t.rx_data[2];
    data[1] = t.rx_data[3]; 
    return err;  
}

esp_err_t BMI323::action_read_multi(const uint8_t start_addr, uint8_t* data,
                                    size_t length) const {
    uint8_t CMD_and_addr = static_cast<uint8_t>(0b1000'0000) 
                           | start_addr;
    constexpr uint8_t kDummy = 0b0000'0000;
    uint8_t tx_buffer[2] = {CMD_and_addr, kDummy};
    uint8_t temp[length + 2];
    spi_transaction_t t {
        .length = 16 + (length * 8),
        .rxlength = (length * 8),
        .tx_buffer = tx_buffer,
        .rx_buffer = temp
    };
    esp_err_t err = spi_.action_polling_tx(t); 

    for (size_t i = 0; i < length; i++) {
        data[i] = temp[2 + i];
    }
    return err;
}

void IRAM_ATTR BMI323::vTask(void *pvParameters) {
    BMI323* instance = static_cast<BMI323*>(pvParameters);

    int16_t acc_data[3];
    int16_t gyr_data[3];
    for (;;) {
        instance->get_accelerometer_data(acc_data);
        instance->get_gyro_data(gyr_data);
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}