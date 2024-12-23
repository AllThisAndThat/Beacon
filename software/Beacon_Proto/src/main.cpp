#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "reserved_objects.h"
#include "status_led.h"
#include "i2c.h"

//vTaskDelay(1000/portTICK_PERIOD_MS);
extern "C" {void app_main();}



void app_main() {
    StatusLed statusLed = StatusLed();
    statusLed.initiate();
    statusLed.set_color(Color::kYellow);
    statusLed.set_state(StatusLedState::kBlink);

    I2c port0 = I2c(I2cPort::kPort0);

    esp_err_t err;
    err = port0.initstate();
    if (err != ESP_OK) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kRed);
        vTaskDelay(2000/portTICK_PERIOD_MS);
        abort();
    }

    i2c_master_dev_handle_t hDevice1;
    err = port0.action_add_device(reserved_LTR_303::kDeviceCfg, hDevice1);
    if (err != ESP_OK) {
        statusLed.set_state(StatusLedState::kSolid);
        statusLed.set_color(Color::kRed);
        vTaskDelay(4000/portTICK_PERIOD_MS);
        abort();
    }


    statusLed.set_state(StatusLedState::kSolid);
    for (;;) {
        err = port0.action_probe(reserved_LTR_303::kAddr);
        if (err == ESP_OK) {
            statusLed.set_color(Color::kGreen);
        }
        else {
            statusLed.set_color(Color::kRed);
        }
        vTaskDelay(50/portTICK_PERIOD_MS);
    }

}