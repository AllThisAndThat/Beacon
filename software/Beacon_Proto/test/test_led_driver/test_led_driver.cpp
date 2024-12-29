#include <unity.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "reserved_objects.h"

#include "led_driver.h"

extern "C" {void app_main(int argc, char **argv);}

LedDriver Led = LedDriver();

void test_led_driver_initiate() {
    constexpr ledc_timer_t kTimer0 = LEDC_TIMER_0;
    constexpr ledc_channel_t kChannel0 = LEDC_CHANNEL_0;
    esp_err_t err = Led.initiate(reserved_pin::kLedDriverTestLed, kTimer0, 
        kChannel0, Logic::kLow);
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_set_duty() {
    constexpr uint32_t kDuty = 500;
    esp_err_t err = Led.set_duty(kDuty);
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_action_off() {
    esp_err_t err = Led.action_off();
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_action_on() {
    esp_err_t err = Led.action_on();
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void app_main(int argc, char **argv)
{
    vTaskDelay(1000/portTICK_PERIOD_MS);
    UNITY_BEGIN();
    RUN_TEST(test_led_driver_initiate);
    RUN_TEST(test_set_duty);
    RUN_TEST(test_action_off);
    RUN_TEST(test_action_on);
    UNITY_END();
}