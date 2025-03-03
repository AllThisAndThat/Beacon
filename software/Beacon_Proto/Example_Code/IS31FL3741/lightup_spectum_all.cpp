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
    if (err != ESP_OK) { failure(); }

    err = is31fl3741.action_verify();
    if (err != ESP_OK) { failure(); }

    err = is31fl3741.set_global_current(0x01);
    if (err != ESP_OK) { failure(); }

    err = is31fl3741.action_on();
    if (err != ESP_OK) { failure(); }

    LedConfig config = {0, 0, 0, 0, 0};
    uint8_t period = 1;
    uint8_t increment = 10;
    for (;;) {
        increment += 5;
        for (int color = 0; color < 3; color++) {
            for (uint8_t x = 0; x < 13; x++) {
                if (color == 0) {
                    config.r += increment;
                }
                else if (color == 1) {
                    config.b += increment;
                }
                else {
                    config.g += increment;
                }
                config.x = x;
                for (uint8_t y = 0; y < 9; y++) {
                    config.y = y;
                    err = is31fl3741.set_rgb_led(config);
                    if (err != ESP_OK) {
                        failure();
                    }
                    vTaskDelay(period / portTICK_PERIOD_MS);

                }
            }
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
    vTaskDelay(2000/portTICK_PERIOD_MS);
}

void pass() {
    statusLed.set_state(StatusLedState::kSolid);
    statusLed.set_color(Color::kGreen);
}