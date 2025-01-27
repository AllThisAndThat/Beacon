#include <unity.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "reserved_objects.h"

#include "ltr_303als.h"


extern "C" {void app_main(int argc, char **argv);}

Ltr_303als amb_bright_sensor = Ltr_303als();

void test_ltr_303als_initiate() {
    esp_err_t err; 
    err = amb_bright_sensor.initiate();
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_action_verify() {
    esp_err_t err;
    err = amb_bright_sensor.action_verify();
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_get_brightness() {
    uint32_t brightness;
    brightness = amb_bright_sensor.get_brightness();
    constexpr uint32_t kMin = 5;
    constexpr uint32_t kMax = 200'000;
    bool isBetween = ((brightness > kMin) && (brightness < kMax));
    if (!isBetween) {
        printf("\nNot inbetween: %ld\n", brightness);
    }
    TEST_ASSERT_TRUE(isBetween);
}

void app_main(int argc, char **argv)
{
    vTaskDelay(1000/portTICK_PERIOD_MS);
    UNITY_BEGIN();
    RUN_TEST(test_ltr_303als_initiate);
    RUN_TEST(test_get_brightness);
    UNITY_END();
}