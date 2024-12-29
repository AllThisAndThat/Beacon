#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "reserved_objects.h"
#include "status_led.h"
#include "kts1622.h"

//vTaskDelay(1000/portTICK_PERIOD_MS);
extern "C" {void app_main();}
void failure();
void warning();
void check();

/*
*/
StatusLed statusLed = StatusLed();
Kts1622 kts1622 = Kts1622(Kts1622Addr::kDefault);

void app_main() {
    esp_err_t err = ESP_OK;

    statusLed.initiate();
    statusLed.set_color(Color::kGreen);
    statusLed.set_state(StatusLedState::kBlink);

    err = kts1622.initialize();
    if (err != ESP_OK) {failure();}

    err = kts1622.action_verify();
    if (err != ESP_OK) {failure();}

    uint8_t port0_pd_mask = 0b11111111;
    uint8_t port1_pd_mask = 0b11111111;
    err = kts1622.set_pulldown_state(port0_pd_mask, port1_pd_mask);
    if (err != ESP_OK) {failure();}

    // start test here
    constexpr uint8_t num_cycles = 10;
    err = kts1622.set_debounce_state(Kts1622Pin::kAllPin,
                                     Kts1622Pin::kAllPin,
                                     num_cycles);
    if (err != ESP_OK) {failure();}

    kts1622.set_pin_direction(Kts1622Pin::kNoPin, Kts1622Pin::kPin0);
    for (;;) {
        kts1622.set_output_state(Kts1622Pin::kNoPin, Kts1622Pin::kPin0);
        vTaskDelay(500/portTICK_PERIOD_MS);
        kts1622.set_output_state(Kts1622Pin::kNoPin, Kts1622Pin::kNoPin);
        vTaskDelay(500/portTICK_PERIOD_MS);
    }

}


void failure() {
    statusLed.set_state(StatusLedState::kSolid);
    statusLed.set_color(Color::kRed);
    printf("Failure\n");
    vTaskDelay(2000/portTICK_PERIOD_MS);
}

void warning() {
    statusLed.set_state(StatusLedState::kSolid);
    statusLed.set_color(Color::kYellow);
    vTaskDelay(2000/portTICK_PERIOD_MS);
}

void check() {
    esp_err_t err = ESP_OK;
    uint8_t rx_buffer = 0;

    err = kts1622.get_input_port0(rx_buffer);
    if (err != ESP_OK) {failure();}
    printf("Port 0: %d\n", rx_buffer);
    err = kts1622.get_input_port1(rx_buffer);
    if (err != ESP_OK) {failure();}
    printf("Port 1: %d\n", rx_buffer);
    vTaskDelay(100/portTICK_PERIOD_MS);
}