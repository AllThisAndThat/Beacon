#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "status_led.h"
#include "spi.h"

//vTaskDelay(1000/portTICK_PERIOD_MS);
extern "C" {void app_main();}



void app_main() {
    StatusLed statusLed = StatusLed();
    statusLed.initiate();
    statusLed.set_color(Color::kYellow);
    statusLed.set_state(StatusLedState::kBlink);

    Spi spi2 = Spi();
    esp_err_t err;
    err = spi2.initiate();
    if (err != ESP_OK) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kRed);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        abort();
    };

    uint8_t tx_data[4] = {0,0,0,0};
    uint8_t rx_data[4] = {0,0,0,0};

    uint64_t addr = 0x00;
    spi_transaction_t transaction = spi2.action_create_transaction(addr, tx_data, rx_data);
    err = spi2.action_transmit(&transaction);
    if (err != ESP_OK) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kRed);
        vTaskDelay(2000/portTICK_PERIOD_MS);
        abort();
    };

    err = spi2.action_transmit(&transaction);
    if (err != ESP_OK) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kRed);
        vTaskDelay(3000/portTICK_PERIOD_MS);
        abort();
    };

    if (tx_data[4] != 0) {
        statusLed.set_color(Color::kBlue);
    }
    uint8_t correctId = 0x43;
    if (tx_data[4] == correctId) {
        statusLed.set_blink_period(300);
        statusLed.set_color(Color::kGreen);
    }


    for (;;) {
        
    }

}