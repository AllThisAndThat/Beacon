#include <unity.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "reserved_objects.h"

#include "max9867.h"

extern "C" {void app_main(int argc, char **argv);}

Max9867 codec = Max9867();

void test_initiate() {
    esp_err_t err; 
    err = codec.initiate();
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_action_probe() {
    esp_err_t err;
    err = codec.action_probe();
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_action_verify() {
    esp_err_t err;
    err = codec.action_verify();
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void app_main(int argc, char **argv)
{
    vTaskDelay(1000/portTICK_PERIOD_MS);
    UNITY_BEGIN();
    RUN_TEST(test_initiate);
    RUN_TEST(test_action_probe);
    RUN_TEST(test_action_verify);
    UNITY_END();
}