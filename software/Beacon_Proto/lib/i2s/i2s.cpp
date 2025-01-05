#include "i2s.h"

// #include "reserved_objects.h"

I2s::I2s() {
}

I2s::~I2s() {
}

esp_err_t I2s::initiate() {
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    esp_err_t err = i2s_new_channel(&chan_cfg, &hTxChannel_, &hRxChannel_);
    if (err != ESP_OK) {return err;}

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48'000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = GPIO_NUM_7,
            .bclk = GPIO_NUM_15,
            .ws = GPIO_NUM_16,
            .dout = GPIO_NUM_17,
            .din = GPIO_NUM_18,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    err = i2s_channel_init_std_mode(hTxChannel_, &std_cfg);
    if (err != ESP_OK) {return err;}
    err = i2s_channel_init_std_mode(hRxChannel_, &std_cfg);
    if (err != ESP_OK) {return err;}

    err = i2s_channel_enable(hTxChannel_);
    if (err != ESP_OK) {return err;}
    err = i2s_channel_enable(hRxChannel_);
    if (err != ESP_OK) {return err;}

    return err;
    // esp_err_t err = i2s_channel_init_std_mode(hChannel_,
    //                         &reserved_i2s0::kChannelCfg);
}   