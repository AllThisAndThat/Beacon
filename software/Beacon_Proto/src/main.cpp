#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "uart_printer.h"


extern "C" {void app_main();}
void heartbeatSetup();



const char s[] = "test\n";

constexpr gpio_num_t GPIO_OUTPUT_IO_0  = GPIO_NUM_5;
constexpr uint64_t GPIO_OUTPUT_PIN_SEL = (1ULL << GPIO_OUTPUT_IO_0);

void app_main() {
    UartPrinter& hPrinter = UartPrinter::getInstance();
    hPrinter.initiate();

    heartbeatSetup();

    int cnt = 0;

    for (;;) {
        hPrinter.Print(s);

        // Heartbeat
        gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
        cnt++;
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

}


void heartbeatSetup() {
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}