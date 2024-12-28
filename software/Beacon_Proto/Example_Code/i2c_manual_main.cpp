#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "reserved_objects.h"
#include "status_led.h"
#include "i2c.h"

//vTaskDelay(1000/portTICK_PERIOD_MS);
extern "C" {void app_main();}

/*
*/



void app_main() {
    StatusLed statusLed = StatusLed();
    statusLed.initiate();
    statusLed.set_color(Color::kYellow);
    statusLed.set_state(StatusLedState::kBlink);

    I2c port0 = I2c(I2cPort::kPort0);

    esp_err_t err;
    err = port0.initiate();
    if (err != ESP_OK) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kBlue);
        vTaskDelay(2000/portTICK_PERIOD_MS);
        abort();
    }

    vTaskDelay(100/portTICK_PERIOD_MS);

    i2c_master_dev_handle_t hDevice1;
    err = port0.action_add_device(reserved_LTR_303::kDeviceCfg, hDevice1);
    if (err != ESP_OK) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kBlue);
        vTaskDelay(4000/portTICK_PERIOD_MS);
        abort();
    }

    err = port0.action_probe(reserved_LTR_303::kAddr);
    if (err != ESP_OK) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kBlue);
        vTaskDelay(4000/portTICK_PERIOD_MS);
        abort();
    }

    uint8_t part_id = 0x86;
    uint8_t rx_buffer = 0x0;
    port0.action_read(hDevice1, part_id, &rx_buffer);
    if (rx_buffer != 0xA0) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kBlue);
        vTaskDelay(4000/portTICK_PERIOD_MS);
        abort();
    }
    
    // Gain 96
    constexpr uint8_t kGainSetting = 0b000'111'0'1;
    err = port0.action_write(hDevice1, 0x80, kGainSetting);
    if (err != ESP_OK) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kBlue);
        vTaskDelay(4000/portTICK_PERIOD_MS);
        abort();
    }

    port0.action_read(hDevice1, 0x80, &rx_buffer);
    if (rx_buffer != kGainSetting) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kRed);
        vTaskDelay(3000/portTICK_PERIOD_MS);
    }

    // Measurement Rate
    constexpr uint8_t kMeasAndIntegrateRate = 0b00'111'111;
    err = port0.action_write(hDevice1, 0x85, kMeasAndIntegrateRate);
    if (err != ESP_OK) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kRed);
        vTaskDelay(4000/portTICK_PERIOD_MS);
        abort();
    }

    statusLed.set_state(StatusLedState::kOff);
    rx_buffer = 0x0;
    uint32_t data;
    statusLed.set_color(Color::kGreen);

    for (;;) {
        port0.action_read(hDevice1, 0X8C, &rx_buffer);
        if ((rx_buffer & 0b0000100) == 0b0000100) {

            port0.action_read(hDevice1, 0X88, &rx_buffer);
            data = rx_buffer;
            port0.action_read(hDevice1, 0X89, &rx_buffer);
            data = data + (rx_buffer << 8);

            port0.action_read(hDevice1, 0X8A, &rx_buffer);
            data = data + rx_buffer;
            port0.action_read(hDevice1, 0X8B, &rx_buffer);
            data = data + (rx_buffer << 8);

            printf("Data: %ld\n", data);

            statusLed.set_state(StatusLedState::kSolid);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            statusLed.set_state(StatusLedState::kOff);
        }
        else {
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
        
    }

}