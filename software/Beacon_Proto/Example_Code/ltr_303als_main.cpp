#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "reserved_objects.h"
#include "status_led.h"
#include "ltr_303als.h"

//vTaskDelay(1000/portTICK_PERIOD_MS);
extern "C" {void app_main();}

/*
*/


void app_main() {
    esp_err_t err;

    StatusLed statusLed = StatusLed();
    statusLed.initiate();
    statusLed.set_color(Color::kYellow);
    statusLed.set_state(StatusLedState::kBlink);

    Ltr_303als amb_bright_sensor = Ltr_303als();
    err = amb_bright_sensor.initiate();
    if (err != ESP_OK) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kRed);
        vTaskDelay(3000/portTICK_PERIOD_MS);
        abort();
    }

    err = amb_bright_sensor.action_verify();
    if (err != ESP_OK) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kRed);
        vTaskDelay(3000/portTICK_PERIOD_MS);
        abort();
    }

    statusLed.set_state(StatusLedState::kBlink);
    uint32_t brightness;
    for (;;) {
        brightness = amb_bright_sensor.get_brightness();
        printf("Brightness: %d\n", brightness);
        if (brightness > 2000) {
            statusLed.set_color(Color::kYellow);
        }
        else {
            statusLed.set_color(Color::kGreen);
        }

        vTaskDelay(50/portTICK_PERIOD_MS);
    }

}