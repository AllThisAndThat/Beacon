#include "bmi323.h"

#include "reserved_objects.h"


BMI323::BMI323() 
    : spi_(Spi(reserved::BMI323::port)) {

}

BMI323::~BMI323() {

}

esp_err_t BMI323::initiate() {
    esp_err_t err = spi_.initiate();
    if (err != ESP_OK) {return err;}

    err = set_spi_mode();
    if (err != ESP_OK) {return err;}

    return err;
}

esp_err_t BMI323::action_verify() {
    constexpr uint32_t kChipIdAddr = 0x00;
    // SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA
    spi_transaction_t t = {
        .flags    = SPI_TRANS_USE_RXDATA, // Multi-bit spi, changing cmd and addr length flags
        .addr     = kChipIdAddr,
        .rxlength = 8 * 2,
    }; 
    esp_err_t err = spi_.action_polling_tx(t);
    if (err != ESP_OK) {
        printf("verify read broke\n"); //TODO: delete
        return err;
    }

    constexpr uint8_t kCorrectChipId = 0x43;
    if (t.rx_data[0] != kCorrectChipId) {
        printf("Data doesn't match, expected 67, got %d\n", t.rx_data[0]); //TODO: delete
        return ESP_FAIL;
    }

    return err;
}

esp_err_t BMI323::set_spi_mode() {
    constexpr uint32_t kDummyAddr = 0x00;
    // SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA
    spi_transaction_t t = {
        .flags    = SPI_TRANS_USE_RXDATA, // Multi-bit spi, changing cmd and addr length flags
        .addr     = kDummyAddr,
        .rxlength = 8 * 2,
    };

    esp_err_t err = spi_.action_polling_tx(t);

    //TODO: delete
    for (int i = 0; i < sizeof(t.rx_data); i++) {
        printf("Dummy %d: %d\n", i, t.rx_data[i]);
    }

    return err;
}
