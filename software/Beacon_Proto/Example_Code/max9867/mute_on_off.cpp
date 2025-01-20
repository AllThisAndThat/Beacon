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

    err = max9867.action_on();
    if (err != ESP_OK) {
        failure();
    }

    
    max9867.set_dac_gain(6);
    max9867.set_left_adc_gain(0);

    int cycles = 500;
    for (;;) {
        max9867.action_off();
        printf("Off\n");
        for (int i = 0; i < cycles; i++) {
            max9867.test_echo_audio();
        }

        max9867.action_on();
        printf("on, mute\n");
        for (int i = 0; i < cycles; i++) {
            max9867.test_echo_audio();
        }

        max9867.action_on();
        printf("on, double\n");
        for (int i = 0; i < cycles; i++) {
            max9867.test_echo_audio();
        }

        max9867.action_unmute();
        printf("Mute, on\n");
        for (int i = 0; i < cycles; i++) {
            max9867.test_echo_audio();
        }
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