#include <unity.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "reserved_objects.h"

#include "kts1622.h"

extern "C" {void app_main(int argc, char **argv);}

Kts1622 kts1622 = Kts1622(Kts1622Addr::kDefault);

void test_kts1622_driver_initiate() {
    esp_err_t err = kts1622.initialize();
    TEST_ASSERT_EQUAL(ESP_OK, err);
    vTaskDelay(1/portTICK_PERIOD_MS);
}

void test_kts1622_driver_action_verify() {
    esp_err_t err = kts1622.action_verify();
    TEST_ASSERT_EQUAL(ESP_OK, err);
    vTaskDelay(1/portTICK_PERIOD_MS);
}

void test_kts1622_driver_set_pin_direction() {
    esp_err_t err = kts1622.set_pin_direction(Kts1622Pin::kNoPin,
                                              Kts1622Pin::kPin0);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    vTaskDelay(1/portTICK_PERIOD_MS);
}


void test_kts1622_driver_set_debounce_state() {
    constexpr uint8_t num_cycles = 10;
    esp_err_t err = kts1622.set_debounce_state(Kts1622Pin::kAllPin,
                                               Kts1622Pin::kAllPin,
                                               num_cycles);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    vTaskDelay(1/portTICK_PERIOD_MS);
}

void test_kts1622_driver_set_pullup_state() {
    uint8_t port0_pd_mask = 0b11111111;
    uint8_t port1_pd_mask = 0b11111111;
    esp_err_t err = kts1622.set_pullup_state(port0_pd_mask,
                                             port1_pd_mask);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    vTaskDelay(1/portTICK_PERIOD_MS);
}

void test_kts1622_driver_set_pulldown_state() {
    uint8_t port0_pd_mask = 0b11111111;
    uint8_t port1_pd_mask = 0b11111111;
    esp_err_t err = kts1622.set_pulldown_state(port0_pd_mask,
                                               port1_pd_mask);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    vTaskDelay(1/portTICK_PERIOD_MS);
}

void test_kts1622_driver_set_output_state() {
    esp_err_t err = kts1622.set_output_state(Kts1622Pin::kNoPin,
                                             Kts1622Pin::kPin0);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    vTaskDelay(1/portTICK_PERIOD_MS);
}

void test_kts1622_driver_set_interrupt_state() {
    esp_err_t err = kts1622.set_interrupt_state(Kts1622Pin::kNoPin,
                                                Kts1622Pin::kNoPin);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    vTaskDelay(1/portTICK_PERIOD_MS);
}

void test_kts1622_driver_get_input_port0() {
    uint8_t input_state = 0;
    esp_err_t err = kts1622.get_input_port0(input_state);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    vTaskDelay(1/portTICK_PERIOD_MS);
}

void test_kts1622_driver_get_input_port1() {
    uint8_t input_state = 0;
    esp_err_t err = kts1622.get_input_port1(input_state);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    vTaskDelay(1/portTICK_PERIOD_MS);
}

void test_kts1622_driver_action_sw_reset() {
    esp_err_t err = kts1622.action_sw_reset();
    TEST_ASSERT_EQUAL(ESP_OK, err);
    vTaskDelay(1/portTICK_PERIOD_MS);
}

void app_main(int argc, char **argv)
{
    vTaskDelay(1000/portTICK_PERIOD_MS);
    UNITY_BEGIN();
    RUN_TEST(test_kts1622_driver_initiate);
    RUN_TEST(test_kts1622_driver_action_verify);
    RUN_TEST(test_kts1622_driver_set_pin_direction);
    RUN_TEST(test_kts1622_driver_set_debounce_state);
    RUN_TEST(test_kts1622_driver_set_pullup_state);
    RUN_TEST(test_kts1622_driver_set_pulldown_state);
    RUN_TEST(test_kts1622_driver_set_output_state);
    RUN_TEST(test_kts1622_driver_set_interrupt_state);
    RUN_TEST(test_kts1622_driver_get_input_port0);
    RUN_TEST(test_kts1622_driver_get_input_port1);
    RUN_TEST(test_kts1622_driver_action_sw_reset);
    UNITY_END();
}

