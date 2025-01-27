#include <unity.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "uart_printer.h"

extern "C" {void app_main(int argc, char **argv);}

UartPrinter printer = UartPrinter();

void test_uart_printer_initiate() {
    esp_err_t err = printer.initiate();
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_empty_string_print() {
    char s[] = "";
    esp_err_t err = printer.action_print(s);
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_empty_line_print() {
    char s[] = "/n";
    esp_err_t err = printer.action_print(s);
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_long_string_print() {
    char s[] = "extremelylonglineextremelylonglineextremely"
    "longlineextremelylonglineextremelylonglineextremelylong"
    "lineextremelylonglinelineextremelylonglinelineextremely\n";
    esp_err_t err = printer.action_print(s);
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_large_pos_num_print() {
    int n = 2'147'483'647;
    esp_err_t err = printer.action_print(n);
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_large_neg_num_print() {
    int n = -2'147'483'646;
    esp_err_t err = printer.action_print(n);
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_zero_print() {
    int n = 0;
    esp_err_t err = printer.action_print(n);
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void test_pair_print() {
    int n = 712;
    char s[] = "Variable";
    esp_err_t err = printer.action_print_pair(s, n);
    TEST_ASSERT_EQUAL(ESP_OK, err);
}


void app_main(int argc, char **argv)
{
    vTaskDelay(1000/portTICK_PERIOD_MS);
    UNITY_BEGIN();
    RUN_TEST(test_uart_printer_initiate);
    RUN_TEST(test_empty_string_print);
    RUN_TEST(test_empty_line_print);
    RUN_TEST(test_long_string_print);
    RUN_TEST(test_large_pos_num_print);
    RUN_TEST(test_large_neg_num_print);
    RUN_TEST(test_zero_print);
    RUN_TEST(test_pair_print);

    UNITY_END();
}