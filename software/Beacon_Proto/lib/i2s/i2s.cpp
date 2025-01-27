#include "i2s.h"

#include "reserved_objects.h"

I2s::I2s() {
}

I2s::~I2s() {
}

esp_err_t I2s::initiate() {
    using namespace reserved::i2s0;
    esp_err_t err = i2s_new_channel(&kChanCfg, &hTxChannel_,
                                    &hRxChannel_);
    if (err != ESP_OK) {return err;}

    err = i2s_channel_init_std_mode(hTxChannel_, &kStdCfg);
    if (err != ESP_OK) {return err;}
    err = i2s_channel_init_std_mode(hRxChannel_, &kStdCfg);
    if (err != ESP_OK) {return err;}

    err = i2s_channel_enable(hTxChannel_);
    if (err != ESP_OK) {return err;}
    err = i2s_channel_enable(hRxChannel_);
    if (err != ESP_OK) {return err;}

    return err;
}   