#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "reserved_objects.h"
#include "status_led.h"

// Custom includes
#include "max9867.h"

//vTaskDelay(1000/portTICK_PERIOD_MS);
extern "C" {void app_main();}
void failure();
void warning();
void pass();

StatusLed statusLed = StatusLed();

// Custom instances
Max9867 max9867 = Max9867();

void app_main() {
    esp_err_t err;

    statusLed.initiate();
    statusLed.set_color(Color::kGreen);
    statusLed.set_state(StatusLedState::kBlink);

// Custom initiates
    err = max9867.initiate();
    if (err != ESP_OK) {
        failure();
    }

    err = max9867.action_verify();
    if (err != ESP_OK) {
        failure();
    }
    JackState state;
    for (;;) {
        err = max9867.get_hp_jack_state(state);
        if (err == ESP_OK) {
            printf("Jack State: %d\n", static_cast<uint8_t>(state));
        }
        else {
            warning();
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }

}

void failure() {
    statusLed.set_state(StatusLedState::kSolid);
    statusLed.set_color(Color::kRed);
    printf("Failure\n");
    vTaskDelay(2000/portTICK_PERIOD_MS);
}

void warning() {
    statusLed.set_state(StatusLedState::kBlink);
    statusLed.set_color(Color::kYellow);
}

void pass() {
    statusLed.set_state(StatusLedState::kSolid);
    statusLed.set_color(Color::kGreen);
}