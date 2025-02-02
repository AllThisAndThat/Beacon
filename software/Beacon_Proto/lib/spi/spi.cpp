#include "spi.h"

#include "reserved_objects.h"

Spi::Spi(spi_host_device_t port) 
    : hSpi_(nullptr) {
    port_ = port;
}

Spi::~Spi() {

}

esp_err_t Spi::initiate() {
    esp_err_t err = ESP_OK;
    if (port_ == SPI2_HOST) {
        using namespace reserved::spi2;
        err = spi_bus_initialize(kSpiNum, &kBusCfg, SPI_DMA_CH_AUTO);
        if (err != ESP_OK) {return ESP_OK;}

        err = spi_bus_add_device(kSpiNum, &kDeviceCfg, &hSpi_);
    }

    return err;
}

esp_err_t Spi::action_polling_tx(spi_transaction_t& t) const {
    esp_err_t err = spi_device_acquire_bus(hSpi_, portMAX_DELAY);
    if (err != ESP_OK) {return ESP_OK;}

    // spi_device_transmit
    err = spi_device_polling_transmit(hSpi_, &t); // TODO: not thread safe
    if (err != ESP_OK) {return ESP_OK;}

    spi_device_release_bus(hSpi_);
    return err;
}
