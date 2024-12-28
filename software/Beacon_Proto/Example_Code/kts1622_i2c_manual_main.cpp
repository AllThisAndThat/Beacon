#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "reserved_objects.h"
#include "status_led.h"
#include "i2c.h"

//vTaskDelay(1000/portTICK_PERIOD_MS);
extern "C" {void app_main();}
void failure();
void warning();

/*
*/
StatusLed statusLed = StatusLed();

void app_main() {
    esp_err_t err = ESP_OK;

    statusLed.initiate();
    statusLed.set_color(Color::kGreen);
    statusLed.set_state(StatusLedState::kBlink);

    I2c port1 = I2c(I2cPort::kPort1);
    err = port1.initiate();
    if (err != ESP_OK) {failure();}

    i2c_master_dev_handle_t hDevice1;
    err = port1.action_add_device(reserved_KTS1622::kDeviceCfg, hDevice1);
    if (err != ESP_OK) {failure();}

    err = port1.action_probe(reserved_KTS1622::kAddr);
    if (err != ESP_OK) {failure();}

    constexpr uint8_t rInputPort0State = 0x00;
    uint8_t rx_buffer = 0;
    for (;;) {
        err = port1.action_read(hDevice1, rInputPort0State, &rx_buffer);
        if (err != ESP_OK) {warning();}
        else {statusLed.set_color(Color::kGreen);}
        printf("Data: %d\n", rx_buffer);
        vTaskDelay(50/portTICK_PERIOD_MS);
    }

}


void failure() {
    statusLed.set_state(StatusLedState::kSolid);
    statusLed.set_color(Color::kRed);
    printf("Failure\n");
    vTaskDelay(2000/portTICK_PERIOD_MS);
    abort();
}

void warning() {
    statusLed.set_state(StatusLedState::kSolid);
    statusLed.set_color(Color::kYellow);
    vTaskDelay(2000/portTICK_PERIOD_MS);
}