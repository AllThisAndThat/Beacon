#include "spi.h"

#include "reserved_objects.h"

Spi::Spi() {

}

Spi::~Spi() {

}

esp_err_t Spi::initiate() {
    esp_err_t err;
    
    using namespace reserved_spi2;
    err = spi_bus_initialize(SPI2_HOST, &kBusCfg, SPI_DMA_CH_AUTO);
    if (err != ESP_OK) {return err;}
    err = spi_bus_add_device(SPI2_HOST, &kDeviceCfg, &hDevice);
    return err;
}

spi_transaction_t Spi::action_create_transaction(uint64_t address, uint8_t tx_data[4], uint8_t rx_data[4]) {
    // SPI_SWAP_DATA_TX, SPI_SWAP_DATA_RX Macro to convert to uint8_t type
    // can maybe create these compile time constexpr
    spi_transaction_t transaction_cfg = {
        .flags    = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA, // Multi-bit spi, changing cmd and addr length flags
        .addr     = address,
        .length   = reserved_spi2::tx_length,
        .rxlength = reserved_spi2::rx_length,
        .tx_data  = *tx_data, // Must be DMA capable memory and 32-bit aligned
        .rx_data  = *rx_data
    };
    return transaction_cfg;
}

esp_err_t Spi::action_transmit(spi_transaction_t *transaction) const {
    return spi_device_polling_transmit(this->hDevice, transaction);
}
