#include "status_led.h"

#include "reserved_objects.h"

StatusLed::StatusLed() {
    red_led   = LedDriver();
    green_led = LedDriver();
    blue_led  = LedDriver();

    state = StatusLedState::kOff;

    constexpr uint64_t kDefaultHalfPeriod = 500;
    half_period_ms = kDefaultHalfPeriod;
}

StatusLed::~StatusLed() {
    vTaskDelete(hTask);
}

esp_err_t StatusLed::initiate() {
    esp_err_t err;
    err = red_led.initiate(reserved_pin::kRedStatusLed, 
                           reserved_ledc::kStatusLeds,
                           reserved_ledc::kRedLed);
    
    if (err != ESP_OK) {return err;}
    err = green_led.initiate(reserved_pin::kGreenStatusLed,
                             reserved_ledc::kStatusLeds,
                             reserved_ledc::kGreenLed);
    if (err != ESP_OK) {return err;}
    err = blue_led.initiate(reserved_pin::kBlueStatusLed,
                            reserved_ledc::kStatusLeds,
                            reserved_ledc::kBlueLed);
    if (err != ESP_OK) {return err;}

    constexpr configSTACK_DEPTH_TYPE kStackSize = 20'000;
    constexpr UBaseType_t kTaskPriority = 10; // Might want to set priority in reservered_objects
    xTaskCreate(static_cast<TaskFunction_t>(this->vTask), 
                "status_led", kStackSize, this, kTaskPriority,
                &hTask);
    return err;
}

void StatusLed::set_state(StatusLedState state) {
    this->state = state;
}

esp_err_t StatusLed::set_color(Color color) {
    // TODO: setup errors, remove magic numbers - brightness
    using enum Color;
    switch (color) {
    case kRed:
        red_led.set_duty(1000);
        green_led.set_duty(0);
        blue_led.set_duty(0);
        break;
    case kYellow:
        red_led.set_duty(500);
        green_led.set_duty(500);
        blue_led.set_duty(0);
        break;
    case kGreen:
        red_led.set_duty(0);
        green_led.set_duty(1000);
        blue_led.set_duty(0);
        break;        
    case kBlue:
        red_led.set_duty(0);
        green_led.set_duty(0);
        blue_led.set_duty(1000);
        break;
    }
    return ESP_OK;
}

void StatusLed::set_blink_period(uint64_t period) {
    half_period_ms = period / 2;
}

esp_err_t StatusLed::action_on() {
    esp_err_t err = red_led.action_on();
    if (err != ESP_OK) {return err;}
    err = green_led.action_on();
    if (err != ESP_OK) {return err;}
    return  blue_led.action_on();
}

esp_err_t StatusLed::action_off() {
    esp_err_t err = red_led.action_off();
    if (err != ESP_OK) {return err;}
    err = green_led.action_off();
    if (err != ESP_OK) {return err;}
    return blue_led.action_off();
}

void StatusLed::vTask(void *pvParameters) {
    StatusLed* instance = static_cast<StatusLed*>(pvParameters);

    constexpr uint32_t kRefreshRate = 100 / portTICK_PERIOD_MS;
    using enum StatusLedState;
    for (;;) {
        switch (instance->state) {
        case kOff:
            instance->action_off();
            vTaskDelay(kRefreshRate);
            break;
        case kSolid:
            instance->action_on();
            vTaskDelay(kRefreshRate);
            break;
        case kBlink:
            instance->action_on();
            vTaskDelay(instance->half_period_ms/portTICK_PERIOD_MS);
            instance->action_off();
            vTaskDelay(instance->half_period_ms/portTICK_PERIOD_MS);
            break;
        }
    }
}