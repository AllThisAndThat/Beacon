#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "reserved_objects.h"
#include "status_led.h"

// Custom includes
#include "is31fl3741.h"
#include "cstring"

//vTaskDelay(1000/portTICK_PERIOD_MS);
extern "C" {void app_main();}
void failure();
void warning();
void pass();

StatusLed statusLed = StatusLed();

// Custom instances
IS31FL3741 is31fl3741 = IS31FL3741();


void app_main() {
    esp_err_t err;

    statusLed.initiate();
    statusLed.set_color(Color::kGreen);
    statusLed.set_state(StatusLedState::kBlink);

// Custom initiates
    err = is31fl3741.initiate();
    if (err != ESP_OK) { failure();}

    err = is31fl3741.action_verify();
    if (err != ESP_OK) { failure();}

    err = is31fl3741.set_global_current(0x0A);
    if (err != ESP_OK) { failure();}

    err = is31fl3741.action_on();
    if (err != ESP_OK) { failure();}

    is31fl3741.action_reset_all_leds();

    uint16_t led_num = 0;
    for (;;) {
        for (led_num = 0; led_num < 351; led_num = led_num + 3) {
            err = is31fl3741.set_specific_led(led_num, 0x20, 0x20);
            if (err != ESP_OK) { failure();}
            printf("LED: %d\n", led_num);
            vTaskDelay(20/portTICK_PERIOD_MS);
        }
        vTaskDelay(1000/portTICK_PERIOD_MS);
        is31fl3741.action_reset_all_leds();
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
    vTaskDelay(2000/portTICK_PERIOD_MS);
}

void pass() {
    statusLed.set_state(StatusLedState::kSolid);
    statusLed.set_color(Color::kGreen);
}