#include "ltr_303als.h"
#include "reserved_objects.h"

Ltr_303als::Ltr_303als() 
    : i2c(reserved_LTR_303::kI2cPort)
{

}

Ltr_303als::~Ltr_303als() {

}

esp_err_t Ltr_303als::initiate() {
    esp_err_t err = i2c.initiate();
    if (err != ESP_OK) {return err;}

    err = i2c.action_add_device(reserved_LTR_303::kDeviceCfg, hDevice);
    if (err != ESP_OK) {return err;}

    constexpr uint8_t rGainSetting = 0x80;
    constexpr uint8_t kMaxGainSetting = 0b000'111'0'1; // 96X gain, activates sensor
    err = i2c.action_write_reg(hDevice, rGainSetting, kMaxGainSetting);
    if (err != ESP_OK) {return err;}
    uint8_t rx_buffer = 0x0;
    i2c.action_read(hDevice, rGainSetting, &rx_buffer);
    if (rx_buffer != kMaxGainSetting) {
        return ESP_ERR_INVALID_RESPONSE;
    }
    
    constexpr uint8_t rMeasIntRate = 0x85;
    constexpr uint8_t kMeasAndIntRate = 0b00'000'011; // 100 ms integration time, 500 ms measurement rate
    err = i2c.action_write_reg(hDevice, rMeasIntRate, kMeasAndIntRate);
    i2c.action_read(hDevice, rMeasIntRate, &rx_buffer);
    if (rx_buffer != kMeasAndIntRate) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    constexpr configSTACK_DEPTH_TYPE kStackSize = 20'000;
    constexpr UBaseType_t kTaskPriority = 10; // Might want to set priority in reservered_objects
    xTaskCreate(static_cast<TaskFunction_t>(this->vTask), 
                "brightness_read", kStackSize, this, kTaskPriority,
                &hTask);

    return ESP_OK;
}

uint32_t Ltr_303als::get_brightness() {
    return this->brightness;
}

esp_err_t Ltr_303als::action_verify() {
    uint8_t rx_buffer = 0x0;

    constexpr uint8_t rPartId = 0x86;
    esp_err_t err = i2c.action_read(hDevice, rPartId, &rx_buffer);
    if (err != ESP_OK) {return err;}
    constexpr uint8_t kCorrectPartId = 0xA0;
    if (rx_buffer != kCorrectPartId) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    constexpr uint8_t rManufactureID = 0x87;
    err = i2c.action_read(hDevice, rManufactureID, &rx_buffer);
    if (err != ESP_OK) {return err;}
    constexpr uint8_t kCorrectManufactureId = 0x05;
    if (rx_buffer != kCorrectManufactureId) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    return ESP_OK;
}

void Ltr_303als::vTask(void *pvParameters) {
    Ltr_303als* instance = static_cast<Ltr_303als*>(pvParameters);
    I2c &i2c = instance->i2c;
    i2c_master_dev_handle_t &hDevice = instance->hDevice;

    constexpr uint32_t kTaskPeriod = 500 / portTICK_PERIOD_MS;
    constexpr uint8_t rChan1_LSB = 0X88;
    constexpr uint8_t rChan1_MSB = 0X89;
    constexpr uint8_t rChan0_LSB = 0X8A;
    constexpr uint8_t rChan0_MSB = 0X8B;
    uint32_t data;
    uint8_t rx_buffer = 0;
    for (;;) {
        i2c.action_read(hDevice, rChan1_LSB, &rx_buffer);
        data = rx_buffer;
        i2c.action_read(hDevice, rChan1_MSB, &rx_buffer);
        data = data + (rx_buffer << 8);

        i2c.action_read(hDevice, rChan0_LSB, &rx_buffer);
        data = data + rx_buffer;
        i2c.action_read(hDevice, rChan0_MSB, &rx_buffer);
        instance->brightness = data + (rx_buffer << 8);

        vTaskDelay(kTaskPeriod);
    }
}