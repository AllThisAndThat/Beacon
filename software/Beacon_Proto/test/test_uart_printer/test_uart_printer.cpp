#include <unity.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "uart_printer.h"

extern "C" {void app_main(int argc, char **argv);}

/* List of things to test
* Copies are not allowed
* Destructor works
* Printing strings in all cases works
* 
* Never call destructor, once initialized, always initialized
*/


void test_uart_printer_initiate() {
    UartPrinter& hPrinter = UartPrinter::getInstance();
    TEST_ASSERT_EQUAL(ESP_OK, hPrinter.initiate());
}

void test_empty_string_print() {
    char s[] = "";
    UartPrinter& hPrinter = UartPrinter::getInstance();
    TEST_ASSERT_EQUAL(0, hPrinter.Print(s));
    vTaskDelay(10/portTICK_PERIOD_MS);
}

void test_empty_line_print() {
    char s[] = "/n";
    UartPrinter& hPrinter = UartPrinter::getInstance();
    TEST_ASSERT_EQUAL(2, hPrinter.Print(s));
    vTaskDelay(10/portTICK_PERIOD_MS);
}

void test_long_string_print() {
    char s[] = "extremelylonglineextremelylonglineextremely"
    "longlineextremelylonglineextremelylonglineextremelylong"
    "lineextremelylonglinelineextremelylonglinelineextremely/n";
    UartPrinter& hPrinter = UartPrinter::getInstance();
    TEST_ASSERT_EQUAL(155, hPrinter.Print(s));
    vTaskDelay(10/portTICK_PERIOD_MS);
}

// void test_unintialized_print() {
//     char s[] = "test/n";

// }


void app_main(int argc, char **argv)
{
    vTaskDelay(1000/portTICK_PERIOD_MS);
    UNITY_BEGIN();
    RUN_TEST(test_uart_printer_initiate);
    RUN_TEST(test_empty_string_print);
    RUN_TEST(test_empty_line_print);
    RUN_TEST(test_long_string_print);
    UNITY_END();
}