#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "reserved_objects.h"
#include "status_led.h"

// Custom includes
#include "cap1188.h"

//vTaskDelay(1000/portTICK_PERIOD_MS);
extern "C" {void app_main();}
void failure();
void warning();
void pass();

StatusLed statusLed = StatusLed();

// Custom instances
CAP1188 cap1188 = CAP1188();


void app_main() {
    esp_err_t err;

    statusLed.initiate();
    statusLed.set_color(Color::kGreen);
    statusLed.set_state(StatusLedState::kBlink);

// Custom initiates
    err = cap1188.initiate();
    if (err != ESP_OK) { failure();}
    pass();

    for (;;) {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void failure() {
    statusLed.set_state(StatusLedState::kSolid);
    statusLed.set_color(Color::kRed);
    printf("Failure\n");
    vTaskDelay(3000/portTICK_PERIOD_MS);
}

void warning() {
    statusLed.set_state(StatusLedState::kBlink);
    statusLed.set_color(Color::kYellow);
    vTaskDelay(2000/portTICK_PERIOD_MS);
}

void pass() {
    statusLed.set_state(StatusLedState::kSolid);
    statusLed.set_color(Color::kGreen);
}