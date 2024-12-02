#include "uart_printer.h"

#include <string.h>

#include "driver/uart.h"

#include "pin_assignment.h"

constexpr uart_port_t kUartNum = UART_NUM_0;
bool UartPrinter::isInitiated = false;

UartPrinter& UartPrinter::getInstance() {
    static UartPrinter instance;
    return instance;
}

esp_err_t UartPrinter::initiate() {
    esp_err_t err = ESP_OK;

    constexpr uart_config_t kConfig = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
        .flags = {.backup_before_sleep = 0}
    };
    err = uart_param_config(kUartNum, &kConfig);
    if (err != ESP_OK) {
        return err;
    }

    constexpr int kNoPin = -1;
    err = uart_set_pin(kUartNum, pin::kUart0Tx, pin::kUart0Rx, kNoPin, kNoPin);
    if (err != ESP_OK) {
        return err;
    }

    constexpr int kTxBufferSize = (1024 * 2);
    constexpr int kRxBufferSize = (1024 * 2);
    constexpr int kQueueSize = 10;
    constexpr int kNoInterrupt = 0;
    err = uart_driver_install(kUartNum, kTxBufferSize, kRxBufferSize, 
        kQueueSize, this->hUartQueue, kNoInterrupt);
    if (err == ESP_OK) {
        UartPrinter::isInitiated = true;
    }
    return err;
}

int UartPrinter::Print(const char* message) {
    if (!UartPrinter::isInitiated) {
        return -2;
    }
    return uart_write_bytes(kUartNum, message, strlen(message));
}

UartPrinter::~UartPrinter()
{
    uart_driver_delete(kUartNum);
    UartPrinter::isInitiated = false;
}