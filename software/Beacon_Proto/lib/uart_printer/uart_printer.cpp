#include "uart_printer.h"

#include <string.h>

#include "driver/uart.h"

#include "reserved_objects.h"
#include "custom_math.h"

int DigitsNumChars(int x);

UartPrinter::UartPrinter() {

}

UartPrinter::~UartPrinter() {

}

esp_err_t UartPrinter::initiate() const {
    using namespace reserved::uart0;
    esp_err_t err = uart_param_config(kUartNum, &kConfig);
    if (err != ESP_OK) { return err;}

    err = uart_set_pin(kUartNum, kTxPin, kRxPin, kRtsPin, kCtsPin);
    if (err != ESP_OK) { return err;}

    constexpr int kTxBufferSize = (1024 * 2);
    constexpr int kRxBufferSize = (1024 * 2);
    constexpr int kQueueSize = 10;
    constexpr int kNoInterrupt = 0;
    err = uart_driver_install(kUartNum, kTxBufferSize, kRxBufferSize, 
        kQueueSize, NULL, kNoInterrupt);
    return err;
}

esp_err_t UartPrinter::action_print(const char* message) const {
    using namespace reserved::uart0;
    size_t message_len = strlen(message);
    int num_written = uart_write_bytes(kUartNum, message,
                                       message_len);
    if (message_len != num_written) {
        return ESP_FAIL;
    }
    else {
        return ESP_OK;
    }
}

esp_err_t UartPrinter::action_print(const int num) const {
    using namespace reserved::uart0;
    int length = DigitsNumChars(num);

    char str_msg[length];
    sprintf(str_msg, "%d", num);
    int num_written = uart_write_bytes(kUartNum, str_msg, length);
    uart_write_bytes(kUartNum, "\n", 1);
    if (length != num_written) {
        return ESP_FAIL;
    }
    else {
        return ESP_OK;
    }
}

esp_err_t UartPrinter::action_print_pair(const char* message, const int num) const {
    using namespace reserved::uart0;

    esp_err_t err = action_print(message);
    if (err != ESP_OK) {return err;}

    uart_write_bytes(kUartNum, ": ", 2);

    err = action_print(num);

    return err;
}