#pragma once

#include "driver/spi_master.h"

#include "reserved_objects.h"

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
    Spi(spi_host_device_t port);
    ~Spi();
    esp_err_t initiate();

    esp_err_t action_polling_tx(spi_transaction_t &t) const;

private:
    spi_device_handle_t hSpi_;
    spi_host_device_t port_;
    spi_device_handle_t hDevice;
};
