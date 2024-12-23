#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "status_led.h"

#include "esp_check.h"

#include "esp_system.h"

//vTaskDelay(1000/portTICK_PERIOD_MS);
extern "C" {void app_main();}

constexpr gpio_num_t GPIO_OUTPUT_IO_0  = GPIO_NUM_5;
constexpr uint64_t GPIO_OUTPUT_PIN_SEL = (1ULL << GPIO_OUTPUT_IO_0);

StatusLed statusLed = StatusLed();

void app_main() {
    statusLed.initiate();
    vTaskDelay(1000/portTICK_PERIOD_MS);

    using enum Color;
    using enum StatusLedState;
    for (;;) {

        statusLed.set_color(kRed);
        statusLed.set_state(kOff);
        vTaskDelay(1000/portTICK_PERIOD_MS);

        statusLed.set_state(kSolid);
        vTaskDelay(5000/portTICK_PERIOD_MS);

        statusLed.set_color(kYellow);
        vTaskDelay(5000/portTICK_PERIOD_MS);

        statusLed.set_state(kBlink);
        vTaskDelay(5000/portTICK_PERIOD_MS);

        statusLed.set_blink_period(200);
        vTaskDelay(5000/portTICK_PERIOD_MS);

        statusLed.set_color(kGreen);
        vTaskDelay(5000/portTICK_PERIOD_MS);
    }

}