#pragma once

#include "freertos/freertos.h"
#include "freertos/queue.h"

class UartPrinter
{
    public:
        static UartPrinter& getInstance();
        int Print(const char* message);
        esp_err_t initiate();

        // Disable copying
        UartPrinter(UartPrinter const &)    = delete;
        void operator=(UartPrinter const &) = delete;
    private:
        UartPrinter() {};
        ~UartPrinter();

        static bool isInitiated;

        QueueHandle_t *hUartQueue;
};