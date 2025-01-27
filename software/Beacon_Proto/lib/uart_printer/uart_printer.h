#pragma once

#include "freertos/freertos.h"
#include "freertos/queue.h"

class UartPrinter
{
    public:
        UartPrinter();
        ~UartPrinter();
        esp_err_t initiate() const;

        esp_err_t action_print(const char* message) const;
        esp_err_t action_print(int message) const;
        esp_err_t action_print_pair(const char* message, const int num) const;
        
    private:

};