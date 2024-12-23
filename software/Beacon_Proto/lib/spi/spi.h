#pragma once

#include "driver/spi_master.h"


/*
Support:
* Custom buffer_size
* reserved objects keep track of which core ISR is pinned to
* SPI3
* CMDs
* Interrupt support
* Back-to-Back transactions
* only supports single device
*/


class Spi {
public:
    Spi();
    ~Spi();
    esp_err_t initiate();
    
    spi_transaction_t action_create_transaction(uint64_t address,
                                                uint8_t tx_data[4],
                                                uint8_t rx_data[4]);
    esp_err_t action_transmit(spi_transaction_t *transaction) const;

private:
    spi_device_handle_t hDevice;
};
