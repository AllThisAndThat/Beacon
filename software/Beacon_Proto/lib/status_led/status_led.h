#pragma once

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

#include "led_driver.h"
/*
Abstract away color and blink/solid -> set mode
Set Mode: enum -> healthy, error, fatal, debug, programming
Set brightness
remove magic numbers 
*/

enum class Color {
    kRed,
    kYellow,
    kGreen,
    kBlue,
};

enum class StatusLedState {
    kOff,
    kSolid,
    kBlink
};

class StatusLed {
public:
    StatusLed();
    ~StatusLed();

    esp_err_t initiate();

    void set_state(StatusLedState state);
    esp_err_t set_color(Color color);
    void set_blink_period(uint64_t period);

private:
    LedDriver red_led;
    LedDriver green_led;
    LedDriver blue_led;

    StatusLedState state;
    uint64_t half_period_ms;

    TaskHandle_t hTask;

    esp_err_t action_on();
    esp_err_t action_off();

    static void vTask(void *pvParameters);
};